/*
 * RiotDb.cpp
 *
 *  Created on: 9 Jan 2018
 *      Author: zal
 */

#include "RiotDb.h"

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

	RiotDb::RiotDb(FilePath dbFilePath, DbManager &dbManager)
	: mDbFilePath(dbFilePath)
	, mDbManager(dbManager)
	, mVersion(0)
	{
	}

	RiotDb::~RiotDb()
	{
	}

	FilePath RiotDb::getDbFilePath() const
	{
	    return mDbFilePath;
	}

	void RiotDb::loadDbFileAndDependencies(size_t dependencyDepth)
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

				RiotDb &db = mDbManager.loadDb(depPath, dependencyDepth + 1);

				mDependencyMap.insert(std::pair<uint16_t, RiotDbRef>(depIndex, db));

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
	}

	AssetPtr RiotDb::getAssetById(AssetType type, RecordId id)
    {
	    switch(type)
	    {
	    case ASSET_TEXTURE:

	    	break;

	    default:
	    	Logger::error() << "Requested unknown asset type " << type << " from database " << mDbFilePath;
	    	throw InvalidArgumentException("Unknown asset type");
	    }
    }

    AssetPtr RiotDb::getAssetByRef(AssetType type, const AssetRef &ref)
    {
    	if(ref.dbIndex == 0)
    	{
    		return this->getAssetById(type, ref.assetId);
    	}

        auto it = mDependencyMap.find(ref.dbIndex);
        if(it == mDependencyMap.end())
        {
        	Logger::error() << "Database index " << ref.dbIndex << " not found in database dependencies";

            throw NotFoundException("Database index not found in dependency table");
        }

        return it->second.get().getAssetById(type, ref.assetId);
    }

} 




