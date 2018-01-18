/*
 * TestMain.cpp
 *
 *  Created on: 30 Dec 2017
 *      Author: zal
 */

#include <iostream>
#include <iomanip>

#include "SrscFile.h"
#include "DbManager.h"


void printUsage()
{
	std::cout
		<< "Usage: opendrakan [options] <path to db file>" << std::endl
		<< "Options:" << std::endl
		<< "    -i <id>    Extract record with ID <id>" << std::endl
		<< std::endl;
}

int main(int argc, char **argv)
{
	if(argc <= 1)
	{
		printUsage();
		return 0;
	}

	std::string filename;
	uint16_t extractRecordId = 0;
	for(int i = 1; i < argc; ++i)
	{
		std::string arg(argv[i]);

		if(arg == "-i")
		{
			if(i == argc - 1)
			{
				std::cout << "i flag needs ID argument" << std::endl;
				printUsage();
				return 1;
			}

			std::string arg2(argv[++i]);
			std::istringstream is(arg2);
			is >> std::hex >> extractRecordId;

		}else
		{
			filename = arg;
		}
	}

	if(filename.empty())
	{
	    std::cout << "Need at least a db file argument." << std::endl;
	    printUsage();
	    return 1;
	}


	try
	{
		od::DbManager dbm;

		dbm.loadDb(od::FilePath(filename));

	}catch(std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
