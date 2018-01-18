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
#include "RiotDb.h"


void srscStat(od::SrscFile &file)
{
	std::cout << "Got SRSC version " << (int32_t)file.getVersion() << " with " << file.getRecordCount() << " records." << std::endl;
	std::cout << "Records:" << std::endl;

	std::cout << std::setw(6) << "Index"
			  << std::setw(6) << "Type"
			  << std::setw(8) << "Size"
			  << std::setw(6) << "RecID"
			  << std::setw(6) << "GruID"
			  << std::setw(24) << "Name"
			  << std::endl;

	auto it = file.getDirectoryBegin();
	while(it != file.getDirectoryEnd())
	{
		od::SrscFile::RecordInfo info = file.getRecordInfo(*it);

		std::cout
			<< std::setw(6) << (it - file.getDirectoryBegin())
			<< std::setw(6) << std::hex << it->type << std::dec
			<< std::setw(8) << it->dataSize
			<< std::setw(6) << std::hex << it->recordId << std::dec
			<< std::setw(6) << std::hex << it->groupId << std::dec;

		if((it->type == 0x302) || ((it->type & 0xff) == 0x01) || (it->type == 0x200))  // sound record
		{
			std::cout << std::setw(24) << info.name;
		}

		std::cout << std::endl;

		++it;
	}
}

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

		std::cout << "Loading database " << filename << std::endl;

		od::RiotDb &db = dbm.loadDb(od::FilePath(filename));

		std::cout << "Successfully loaded database!" << std::endl;

		od::SrscFile &srscFile = db.getResourceContainer(od::ASSET_SEQUENCE);
		srscStat(srscFile);


	}catch(std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
