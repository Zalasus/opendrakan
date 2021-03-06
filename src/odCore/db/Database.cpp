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
#include <odCore/Panic.h>
#include <odCore/StringUtils.h>

#include <odCore/db/DbManager.h>
#include <odCore/db/DependencyTable.h>

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
            factoryPtr = std::make_unique<T>(mDependencyTable, *containerPtr);

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
    , mDependencyTable(std::make_shared<DependencyTable>())
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
		    OD_PANIC() << "Could not open db definition file " + mDbFilePath.str();
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
					OD_PANIC() << "Unsupported database version " << mVersion;
				}

			}else if(std::regex_match(line, results, dependenciesRegex))
			{
				std::istringstream is(results[1]);
				is >> totalDependencyCount;

                mDependencyTable->reserveDependencies(totalDependencyCount);

				readingDependencies = true;

			}else if(std::regex_match(line, results, dependencyDefRegex))
			{
				if(!readingDependencies)
				{
					Logger::warn() << "Found dependency definition before dependencies statement in definition file of database " << getShortName();
                    readingDependencies = true;
				}

				uint32_t depIndex;
				std::istringstream is(results[1]);
				is >> depIndex;

				if(depIndex == 0)
				{
					OD_PANIC() << "Invalid dependency index " << depIndex;
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

				mDependencyTable->addDependency(depIndex, db);

			}else
			{
				OD_PANIC() << "Malformed line in database file: " << line;
			}
		}

        if(mDependencyTable->getDependencyCount() != totalDependencyCount)
        {
            Logger::warn() << "Dependency list of database " << getShortName() << " contains more entries than stated by 'dependencies' statement";
        }

        // now that the database is loaded, create the various asset factories
        _tryOpeningAssetContainer(mModelFactory,    mModelContainer,    ".mod");
        _tryOpeningAssetContainer(mAnimFactory,     mAnimContainer,     ".adb");
        _tryOpeningAssetContainer(mSoundFactory,    mSoundContainer,    ".sdb");
        _tryOpeningAssetContainer(mSequenceFactory, mSequenceContainer, ".ssd");
        _tryOpeningAssetContainer(mTextureFactory,  mTextureContainer,  ".txd");
        _tryOpeningAssetContainer(mClassFactory,    mClassContainer,    ".odb");
	}

    template<>
    std::shared_ptr<Texture> Database::loadAsset<Texture>(od::RecordId id)
    {
        return this->loadTexture(id);
    }

    template<>
    std::shared_ptr<Class> Database::loadAsset<Class>(od::RecordId id)
    {
        return this->loadClass(id);
    }

    template<>
    std::shared_ptr<Model> Database::loadAsset<Model>(od::RecordId id)
    {
        return this->loadModel(id);
    }

    template<>
    std::shared_ptr<Sequence> Database::loadAsset<Sequence>(od::RecordId id)
    {
        return this->loadSequence(id);
    }

    template<>
    std::shared_ptr<Animation> Database::loadAsset<Animation>(od::RecordId id)
    {
        return this->loadAnimation(id);
    }

    template<>
    std::shared_ptr<Sound> Database::loadAsset<Sound>(od::RecordId id)
    {
        return this->loadSound(id);
    }

	std::shared_ptr<Texture> Database::loadTexture(od::RecordId recordId)
	{
		if(mTextureFactory == nullptr)
		{
			OD_PANIC() << "Can't load texture. Database has no texture container";
		}

		return mTextureFactory->getAsset(recordId);
	}

	std::shared_ptr<Class> Database::loadClass(od::RecordId recordId)
	{
		if(mClassFactory == nullptr)
		{
			OD_PANIC() << "Can't load class. Database has no class container";
		}

		return mClassFactory->getAsset(recordId);
	}

	std::shared_ptr<Model> Database::loadModel(od::RecordId recordId)
	{
		if(mModelFactory == nullptr)
		{
			OD_PANIC() << "Can't load model. Database has no model container";
		}

        return mModelFactory->getAsset(recordId);
	}

	std::shared_ptr<Sequence> Database::loadSequence(od::RecordId recordId)
	{
        if(mSequenceFactory == nullptr)
        {
            OD_PANIC() << "Can't load sequence. Database has no sequence container";
        }

        return mSequenceFactory->getAsset(recordId);
	}

	std::shared_ptr<Animation> Database::loadAnimation(od::RecordId recordId)
	{
		if(mAnimFactory == nullptr)
		{
			OD_PANIC() << "Can't load animation. Database has no animation container";
		}

		return mAnimFactory->getAsset(recordId);
	}

	std::shared_ptr<Sound> Database::loadSound(od::RecordId recordId)
    {
        if(mSoundFactory == nullptr)
        {
            OD_PANIC() << "Can't load sound. Database has no sound container";
        }

        return mSoundFactory->getAsset(recordId);
    }

}
