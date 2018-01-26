/*
 * TestMain.cpp
 *
 *  Created on: 30 Dec 2017
 *      Author: zal
 */

#include <iostream>
#include <iomanip>
#include <memory>

#include "ZStream.h"
#include "SrscFile.h"
#include "DbManager.h"
#include "RiotDb.h"
#include "Logger.h"
#include "RiotLevel.h"
#include "StringUtils.h"


void srscStat(od::SrscFile &file)
{
	std::cout << "Got SRSC version " << (int32_t)file.getVersion() << " with " << file.getRecordCount() << " records." << std::endl;
	std::cout << "Records:" << std::endl;

	std::cout << std::setw(6) << "Index"
			  << std::setw(6) << "Type"
			  << std::setw(8) << "Size"
			  << std::setw(6) << "RecID"
			  << std::setw(6) << "GruID"
			  << std::setw(24) << "Data"
			  << std::endl;

	auto it = file.getDirectoryBegin();
	while(it != file.getDirectoryEnd())
	{

		std::cout
			<< std::setw(6) << (it - file.getDirectoryBegin())
			<< std::setw(6) << std::hex << it->type << std::dec
			<< std::setw(8) << it->dataSize
			<< std::setw(6) << std::hex << it->recordId << std::dec
			<< std::setw(6) << std::hex << it->groupId << std::dec;

		std::cout << "  ";

		od::DataReader dr(file.getStreamForRecord(*it));

		for(size_t i = 0; i < it->dataSize; ++i)
		{
			uint8_t b;
			dr >> b;

			printf("%02x ", b); // whatcha gonna do bout it?

			if(i >= 16)
			{
				std::cout << " [...]";
				break;
			}
		}

		std::cout << std::endl;

		++it;
	}
}

void printUsage()
{
	std::cout
		<< "Usage: opendrakan [options] <path to srsc file>" << std::endl
		<< "Options:" << std::endl
		<< "    -i <id>    Extract first record with ID <id>" << std::endl
		<< "    -x         Extract all records" << std::endl
		<< "    -s         Print SRSC statistics" << std::endl
		<< "If no option is given, the file is loaded as a level." << std::endl
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
	bool extract = false;
	bool stat = false;
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

		}else if(arg == "-x")
		{
		    extract = true;

		}else if(arg == "-s")
		{
		    stat = true;

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
		od::Logger::getDefaultLogger().setPrintDebug(false);

		/*od::DbManager dbm;

		std::cout << "Loading database " << filename << std::endl;

		od::RiotDb &db = dbm.loadDb(od::FilePath(filename));

		std::cout << "Successfully loaded database!" << std::endl;

		od::SrscFile &srscFile = db.getResourceContainer(od::ASSET_SOUND);

		if(extract)
		{
            if(extractRecordId > 0)
            {
                od::SrscFile::DirEntry dirEntry = srscFile.getDirectoryEntryByID(extractRecordId);
                srscFile.decompressRecord("out/", dirEntry, true);

                od::AssetPtr classTest = db.getAssetById(od::ASSET_SOUND, extractRecordId);

                std::cout << "The loaded asset has name: " << classTest->getName() << std::endl;

            }else
            {
                srscFile.decompressAll("out/", true);
            }

		}else
		{
		    srscStat(srscFile);
		}*/

		if(stat || extract)
		{
		    od::SrscFile srscFile(filename);

		    if(extract)
            {
                if(extractRecordId > 0)
                {
                    od::SrscFile::DirEntry dirEntry = srscFile.getDirectoryEntryByID(extractRecordId);
                    srscFile.decompressRecord("out/", dirEntry, true);

                    std::cout << "Extracting record " << std::hex << extractRecordId << std::dec << " to out/" << std::endl;

                }else
                {
                    srscFile.decompressAll("out/", true);

                    std::cout << "Extracting all records to out/" << std::endl;
                }

            }else
            {
                srscStat(srscFile);
            }

		}else
		{
		    od::DbManager dbm;
		    od::RiotLevel level(filename, dbm);



		}

	}catch(std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
