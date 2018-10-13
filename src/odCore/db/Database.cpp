/*
 * RiotDb.cpp
 *
 *  Created on: 9 Jan 2018
 *      Author: zal
 */

#include <odCore/db/Database.h>

#include <fstream>
#include <sstream>
#include <regex>

#include <odCore/Logger.h>
#include <odCore/StringUtils.h>
#include <odCore/Exception.h>
#include <odCore/db/DbManager.h>

#define OD_RIOTDB_MAXVERSION 1

namespace odDb
{

	Database::Database(od::FilePath dbFilePath, DbManager &dbManager)
	: mDbFilePath(dbFilePath)
	, mDbManager(dbManager)
	, mVersion(0)
	{
	}

	Database::~Database()
	{
	}

	void Database::loadDbFileAndDependencies(size_t dependencyDepth)
	{
		std::regex versionRegex("\\s*version\\s+(\\d+).*");
		std::regex dependenciesRegex("\\s*dependencies\\s+(\\d+).*");
		std::regex dependencyDefRegex("\\s*(\\d+)\\s+(.*)");
		std::regex commentRegex("\\s*"); // allow empty lines. if we find something like a comment, add it here

		std::ifstream in(mDbFilePath.str(), std::ios::in | std::ios::binary);
		if(in.fail())
		{
		    throw od::IoException("Could not open db definition file " + mDbFilePath.str());
		}

		std::string line;
		bool readingDependencies = false;
		size_t totalDependencyCount = 0;
		size_t dependenciesRead = 0;
		while(std::getline(in, line))
		{
			// getline leaves the CR byte (0x0D) in the string if given windows line endings. remove if it is there
			if(line.size() != 0 && line[line.size() - 1] == 0x0D)
			{
				line.erase(line.size() - 1);
			}

			std::smatch results;

			if(std::regex_match(line, results, commentRegex))
			{
				continue;

			}else if(std::regex_match(line, results, versionRegex))
			{
				std::istringstream is(results[1]);
				is >> mVersion;

				if(mVersion > OD_RIOTDB_MAXVERSION)
				{
					throw od::UnsupportedException("Unsupported database version");
				}

			}else if(std::regex_match(line, results, dependenciesRegex))
			{
				std::istringstream is(results[1]);
				is >> totalDependencyCount;

				readingDependencies = true;

			}else if(std::regex_match(line, results, dependencyDefRegex))
			{
				if(!readingDependencies)
				{
					throw od::Exception("Found dependency definition before dependencies statement");
				}

				if(dependenciesRead >= totalDependencyCount)
                {
                    throw od::Exception("More dependency lines found in db file than stated in 'dependencies' statement");
                }

				uint32_t depIndex;
				std::istringstream is(results[1]);
				is >> depIndex;

				if(depIndex == 0)
				{
					throw od::Exception("Invalid dependency index");
				}

				// note: dependency paths are always stored relative to the path of the db file defining it
				od::FilePath depPath(results[2], mDbFilePath.dir());
				depPath = depPath.adjustCase();

				if(depPath == mDbFilePath)
				{
				    Logger::warn() << "Self dependent database file: " << mDbFilePath;
				    ++dependenciesRead;
				    continue;
				}

				Database &db = mDbManager.loadDb(depPath, dependencyDepth + 1);

				mDependencyMap.insert(std::pair<uint16_t, DbRefWrapper>(depIndex, db));

				++dependenciesRead;

			}else
			{
				throw od::Exception("Malformed line in database file: " + line);
			}
		}

        if(dependenciesRead < totalDependencyCount)
        {
            throw od::Exception("Found less dependency definitions than stated in dependencies statement");
        }


        // now that the database is loaded, create the various asset factories

        _tryOpeningAssetContainer(mModelFactory,    mModelContainer,    ".mod");
        _tryOpeningAssetContainer(mAnimFactory,     mAnimContainer,     ".adb");
        _tryOpeningAssetContainer(mSoundFactory,    mSoundContainer,    ".sdb");
        _tryOpeningAssetContainer(mSequenceFactory, mSequenceContainer, ".ssd");

        // texture container is different. it needs an engine reference
        od::FilePath txdPath = mDbFilePath.ext(".txd");
        if(txdPath.exists())
        {
            mTextureContainer.reset(new od::SrscFile(txdPath));
            mTextureFactory.reset(new TextureFactory(*this, *mTextureContainer, mDbManager.getEngine()));

            Logger::verbose() << "Opened database texture container " << txdPath.str();

        }else
        {
            Logger::verbose() << "Database has no texture container";
        }

        // same with class container
        od::FilePath odbPath = mDbFilePath.ext(".odb");
        if(odbPath.exists())
        {
            mClassContainer.reset(new od::SrscFile(odbPath));
            mClassFactory.reset(new ClassFactory(*this, *mClassContainer, mDbManager.getEngine()));

            Logger::verbose() << "Opened database class container " << odbPath.str();

        }else
        {
            Logger::verbose() << "Database has no class container";
        }
	}

	AssetProvider &Database::getDependency(uint16_t index)
	{
	    auto it = mDependencyMap.find(index);
	    if(it == mDependencyMap.end())
	    {
	        Logger::error() << "Database '" + getShortName() + "' has no dependency with index " << index;
	        throw od::NotFoundException("Database has no dependency with given index");
	    }

	    return it->second;
	}

	Texture *Database::getTexture(od::RecordId recordId)
	{
		if(mTextureFactory == nullptr)
		{
			throw od::NotFoundException("Can't get texture. Database has no texture container");
		}

		osg::ref_ptr<Texture> asset = mTextureFactory->getAsset(recordId);

		return asset.release();
	}

	Class *Database::getClass(od::RecordId recordId)
	{
		if(mClassFactory == nullptr)
		{
			throw od::NotFoundException("Can't get class. Database has no class container");
		}

		osg::ref_ptr<Class> asset = mClassFactory->getAsset(recordId);

		return asset.release();
	}

	Model *Database::getModel(od::RecordId recordId)
	{
		if(mModelFactory == nullptr)
		{
			throw od::NotFoundException("Can't get model. Database has no model container");
		}

		osg::ref_ptr<Model> asset = mModelFactory->getAsset(recordId);

        return asset.release();
	}

	Sequence *Database::getSequence(od::RecordId recordId)
	{
        if(mSequenceFactory == nullptr)
        {
            throw od::NotFoundException("Can't get sequence. Database has no sequence container");
        }

        osg::ref_ptr<Sequence> asset = mSequenceFactory->getAsset(recordId);

        return asset.release();
	}

	Animation *Database::getAnimation(od::RecordId recordId)
	{
		if(mAnimFactory == nullptr)
		{
			throw od::NotFoundException("Can't get animation. Database has no animation container");
		}

		osg::ref_ptr<Animation> asset = mAnimFactory->getAsset(recordId);

		return asset.release();
	}

	Sound *Database::getSound(od::RecordId recordId)
    {
        if(mSoundFactory == nullptr)
        {
            throw od::NotFoundException("Can't get sound. Database has no sound container");
        }

        osg::ref_ptr<Sound> asset = mSoundFactory->getAsset(recordId);

        return asset.release();
    }

} 




