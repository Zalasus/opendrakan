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

namespace odDb
{

    static constexpr uint32_t MAX_DB_VERSION{1};


    template <typename T>
    void Database::_tryOpeningAssetContainer(std::unique_ptr<T> &factoryPtr, std::unique_ptr<od::SrscFile> &containerPtr, const char *extension)
    {
        od::FilePath path = mDbFilePath.ext(extension);
        if(path.exists())
        {
            containerPtr = std::make_unique<od::SrscFile>(path);
            factoryPtr = std::make_unique<T>(*this, *containerPtr);

            Logger::verbose() << AssetTraits<typename T::AssetType>::name() << " container of database opened";

        }else
        {
            Logger::verbose() << "Database has no " << AssetTraits<typename T::AssetType>::name() << " container";
        }
    }


	Database::Database(const od::FilePath &dbFilePath, DbManager &dbManager, GlobalDatabaseIndex globalIndex)
	: mDbFilePath(dbFilePath)
	, mDbManager(dbManager)
    , mGlobalIndex(globalIndex)
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

				if(mVersion > MAX_DB_VERSION)
				{
					throw od::UnsupportedException("Unsupported database version");
				}

			}else if(std::regex_match(line, results, dependenciesRegex))
			{
				std::istringstream is(results[1]);
				is >> totalDependencyCount;

                mDependencyMap.reserve(totalDependencyCount);

				readingDependencies = true;

			}else if(std::regex_match(line, results, dependencyDefRegex))
			{
				if(!readingDependencies)
				{
					throw od::Exception("Found dependency definition before dependencies statement");
				}

				if(mDependencyMap.size() >= totalDependencyCount)
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
				depPath = depPath.adjustCase().ext(".db");

				if(depPath == mDbFilePath)
				{
				    Logger::warn() << "Self dependent database file: " << mDbFilePath;
				    continue;
				}

                // TODO: detect and prevent dependency cycles!!! since Databases now own their dependencies, cycles create leaks
				std::shared_ptr<Database> db = mDbManager.loadDatabase(depPath, dependencyDepth + 1);

				mDependencyMap.insert(std::make_pair(depIndex, db));

			}else
			{
				throw od::Exception("Malformed line in database file: " + line);
			}
		}

        if(mDependencyMap.size() < totalDependencyCount)
        {
            throw od::Exception("Found less dependency definitions than stated in dependencies statement");
        }

        // now that the database is loaded, create the various asset factories
        _tryOpeningAssetContainer(mModelFactory,    mModelContainer,    ".mod");
        _tryOpeningAssetContainer(mAnimFactory,     mAnimContainer,     ".adb");
        _tryOpeningAssetContainer(mSoundFactory,    mSoundContainer,    ".sdb");
        _tryOpeningAssetContainer(mSequenceFactory, mSequenceContainer, ".ssd");
        _tryOpeningAssetContainer(mTextureFactory,  mTextureContainer,  ".txd");
        _tryOpeningAssetContainer(mClassFactory,    mClassContainer,    ".odb");
	}

    std::shared_ptr<Database> Database::getDependencyDatabase(DatabaseIndex index)
    {
        auto it = mDependencyMap.find(index);
	    if(it == mDependencyMap.end())
	    {
	        Logger::error() << "Database '" + getShortName() + "' has no dependency with index " << index;
	        throw od::NotFoundException("Database has no dependency with given index");
	    }

	    return it->second;
    }

    GlobalAssetRef Database::translateLocalToGlobalRef(const AssetRef &ref)
    {
        if(ref.dbIndex == 0)
        {
            return { ref.assetId, this->getGlobalIndex() };

        }else
        {
            auto dep = getDependencyDatabase(ref.assetId);
            return { ref.assetId, dep->getGlobalIndex() };
        }
    }

	AssetProvider &Database::getDependency(uint16_t index)
	{
	    return *getDependencyDatabase(index);
	}

	std::shared_ptr<Texture> Database::getTexture(od::RecordId recordId)
	{
		if(mTextureFactory == nullptr)
		{
			throw od::NotFoundException("Can't get texture. Database has no texture container");
		}

		return mTextureFactory->getAsset(recordId);
	}

	std::shared_ptr<Class> Database::getClass(od::RecordId recordId)
	{
		if(mClassFactory == nullptr)
		{
			throw od::NotFoundException("Can't get class. Database has no class container");
		}

		return mClassFactory->getAsset(recordId);
	}

	std::shared_ptr<Model> Database::getModel(od::RecordId recordId)
	{
		if(mModelFactory == nullptr)
		{
			throw od::NotFoundException("Can't get model. Database has no model container");
		}

        return mModelFactory->getAsset(recordId);
	}

	std::shared_ptr<Sequence> Database::getSequence(od::RecordId recordId)
	{
        if(mSequenceFactory == nullptr)
        {
            throw od::NotFoundException("Can't get sequence. Database has no sequence container");
        }

        return mSequenceFactory->getAsset(recordId);
	}

	std::shared_ptr<Animation> Database::getAnimation(od::RecordId recordId)
	{
		if(mAnimFactory == nullptr)
		{
			throw od::NotFoundException("Can't get animation. Database has no animation container");
		}

		return mAnimFactory->getAsset(recordId);
	}

	std::shared_ptr<Sound> Database::getSound(od::RecordId recordId)
    {
        if(mSoundFactory == nullptr)
        {
            throw od::NotFoundException("Can't get sound. Database has no sound container");
        }

        return mSoundFactory->getAsset(recordId);
    }

}
