/*
 * RiotDb.cpp
 *
 *  Created on: 9 Jan 2018
 *      Author: zal
 */

#include <Database.h>
#include <fstream>
#include <sstream>
#include <regex>

#include "Logger.h"
#include "DbManager.h"
#include "StringUtils.h"
#include "Exception.h"

#define OD_RIOTDB_MAXVERSION 1

namespace od
{

	Database::Database(FilePath dbFilePath, DbManager &dbManager)
	: mDbFilePath(dbFilePath)
	, mDbManager(dbManager)
	, mVersion(0)
	{
	}

	Database::~Database()
	{
	}

	FilePath Database::getDbFilePath() const
	{
	    return mDbFilePath;
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
		    throw IoException("Could not open db definition file " + mDbFilePath.str());
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
					throw UnsupportedException("Unsupported database version");
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
					throw Exception("Found dependency definition before dependencies statement");
				}

				if(dependenciesRead >= totalDependencyCount)
                {
                    throw Exception("More dependency lines found in db file than stated in 'dependencies' statement");
                }

				uint32_t depIndex;
				std::istringstream is(results[1]);
				is >> depIndex;

				if(depIndex == 0)
				{
					throw Exception("Invalid dependency index");
				}

				// note: dependency paths are always stored relative to the path of the db file defining it
				FilePath depPath(results[2], mDbFilePath.dir());
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
				throw Exception("Malformed line in database file: " + line);
			}
		}

        if(dependenciesRead < totalDependencyCount)
        {
            throw Exception("Found less dependency definitions than stated in dependencies statement");
        }


        // now that the database is loaded, create the various asset factories

        FilePath txdPath = mDbFilePath.ext(".txd");
        if(txdPath.exists())
        {
        	mTextureFactory = std::unique_ptr<TextureFactory>(new TextureFactory(txdPath, *this));
        	Logger::verbose() << "Opened database texture container";

        }else
        {
        	Logger::verbose() << "Database has no texture container";
        }

        FilePath modPath = mDbFilePath.ext(".mod");
        if(modPath.exists())
        {
        	mModelFactory = std::unique_ptr<ModelFactory>(new ModelFactory(modPath, *this));
        	Logger::verbose() << "Opened database model container";

        }else
        {
        	Logger::verbose() << "Database has no model container";
        }
	}

	// TODO: these methods do pretty much the same. perhaps replace AssetProvider with a template class that does this automatically?

	TexturePtr Database::getAssetAsTexture(const AssetRef &ref)
	{
		if(ref.dbIndex == 0)
		{
			if(mTextureFactory != nullptr)
			{
				return mTextureFactory->getAsset(ref.assetId);

			}else
			{
				throw NotFoundException("Texture with given ID not found in database");
			}
		}

		auto it = mDependencyMap.find(ref.dbIndex);
		if(it == mDependencyMap.end())
		{
			throw Exception("Database has no dependency with given index");
		}

		AssetRef foreignRef = ref;
		foreignRef.dbIndex = 0;
		return it->second.get().getAssetAsTexture(foreignRef);
	}

	ModelPtr Database::getAssetAsModel(const AssetRef &ref)
	{
		if(ref.dbIndex == 0)
		{
			if(mModelFactory != nullptr)
			{
				return mModelFactory->getAsset(ref.assetId);

			}else
			{
				throw NotFoundException("Model with given ID not found in database");
			}
		}

		auto it = mDependencyMap.find(ref.dbIndex);
		if(it == mDependencyMap.end())
		{
			throw Exception("Database has no dependency with given index");
		}

		AssetRef foreignRef = ref;
		foreignRef.dbIndex = 0;
		return it->second.get().getAssetAsModel(foreignRef);
	}

} 




