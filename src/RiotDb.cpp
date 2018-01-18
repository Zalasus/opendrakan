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
		_loadDbFileAndDependencies();
	}

	FilePath RiotDb::getDbFilePath() const
	{
	    return mDbFilePath;
	}

	void RiotDb::_loadDbFileAndDependencies()
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

				uint32_t depIndex;
				std::istringstream is(results[1]);
				is >> depIndex;

				if(depIndex == 0 || depIndex > mDependencies.size())
				{
					throw Exception("Invalid dependency index");
				}

				// note: dependency paths are always stored relative to the path of the db file defining it
				FilePath depPath(results[2], mDbFilePath.dir());

				if(depPath == mDbFilePath)
				{
				    throw Exception("Self dependent database file");
				}

				mDependencies[depIndex-1] = &mDbManager.loadDb(depPath);

                ++dependenciesLoaded;

			}else
			{
				throw Exception("Malformed line in database file");
			}
		}

        if(dependenciesLoaded < mDependencies.size())
        {
            throw Exception("Found less dependency definitions than stated in dependencies statement");
        }
	}
} 




