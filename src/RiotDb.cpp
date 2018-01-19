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
		for(std::pair<AssetType, SrscFile*> entry : mSrscMap)
		{
			delete entry.second;
		}
	}

	FilePath RiotDb::getDbFilePath() const
	{
	    return mDbFilePath;
	}

	SrscFile &RiotDb::getResourceContainer(AssetType type)
	{
		if(mSrscMap.find(type) == mSrscMap.end())
		{
			// not yet loaded -> load
		    AssetFactory &factory = AssetFactory::getFactoryForType(type);
			std::string srscPath = mDbFilePath.strNoExt() + "." + factory.getDbExtension();

			SrscFile *srscFile = new SrscFile(FilePath(srscPath)); // TODO: use proper RAII
			mSrscMap[type] = srscFile;

			return *srscFile;
		}

		return *mSrscMap[type];
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
		    throw Exception("Could not open db definition file " + mDbFilePath.str());
		}

		std::string line;
		bool readingDependencies = false;
        size_t dependenciesLoaded = 0;
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
					throw Exception("Unsupported database version");
				}

			}else if(std::regex_match(line, results, dependenciesRegex))
			{
				size_t depCount;
				std::istringstream is(results[1]);
				is >> depCount;

				if(depCount > 0)
				{
					mDependencies.allocate(depCount);
				}

				readingDependencies = true;

			}else if(std::regex_match(line, results, dependencyDefRegex))
			{
				if(!readingDependencies)
				{
					throw Exception("Found dependency definition before dependencies statement");
				}

				if(dependenciesLoaded >= mDependencies.size())
                {
                    throw Exception("More dependencies than stated in 'dependencies' statement");
                }

				uint32_t depIndex;
				std::istringstream is(results[1]);
				is >> depIndex;

				if(depIndex == 0)
				{
					throw Exception("Invalid dependency index");
				}

				// the *.db extension is always missing in the dependency definitions. here we make them optional
				std::string depPathStr = results[2];
				if(!StringUtils::endsWith(depPathStr, ".db"))
				{
					depPathStr += ".db";
				}

				// note: dependency paths are always stored relative to the path of the db file defining it
				FilePath depPath(depPathStr, mDbFilePath.dir());

				if(depPath == mDbFilePath)
				{
				    throw Exception("Self dependent database file");
				}

				Dependency dep;
				dep.index = depIndex;

				if(!mDbManager.isDbLoaded(depPath))
				{
					dep.db = &mDbManager.loadDb(depPath, dependencyDepth + 1);

				}else
				{
					dep.db = &mDbManager.getDb(depPath);
				}

				mDependencies[dependenciesLoaded] = dep;

                ++dependenciesLoaded;

			}else
			{
				throw Exception("Malformed line in database file: " + line);
			}
		}

        if(dependenciesLoaded < mDependencies.size())
        {
            throw Exception("Found less dependency definitions than stated in dependencies statement");
        }
	}

	AssetPtr RiotDb::getAssetById(AssetType type, RecordId id)
    {
	    SrscFile &srscFile = getResourceContainer(type);

	    AssetFactory &factory = AssetFactory::getFactoryForType(type);

	    AssetPtr newAsset = factory.createNewAsset();
	    newAsset->loadFromRecord(srscFile, id);

	    return AssetPtr(newAsset);
    }

} 




