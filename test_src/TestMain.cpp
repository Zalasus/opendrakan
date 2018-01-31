/*
 * TestMain.cpp
 *
 *  Created on: 30 Dec 2017
 *      Author: zal
 */

#include <iostream>
#include <iomanip>
#include <memory>
#include <unistd.h>

#include "ZStream.h"
#include "SrscFile.h"
#include "DbManager.h"
#include "RiotDb.h"
#include "Logger.h"
#include "RiotLevel.h"
#include "StringUtils.h"
#include "rfl/RiotFunctionLibrary.h"


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

	const std::vector<od::SrscFile::DirEntry> &directory = file.getDirectory();
	auto it = directory.begin();
	while(it != directory.end())
	{

		std::cout
			<< std::setw(6) << (it - directory.begin())
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
		<< "    -v         Increase verbosity of logger" << std::endl
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

	od::Logger::LogLevel logLevel = od::Logger::LOGLEVEL_INFO;
	std::string filename;
	bool extract = false;
	bool stat = false;
	uint16_t extractRecordId = 0;
	int c;
	while((c = getopt(argc, argv, "i:xsv")) != -1)
	{
		switch(c)
		{
		case 'i':
			{
				std::istringstream iss(optarg);
				iss >> std::hex >> extractRecordId;
				if(iss.fail())
				{
					std::cout << "Argument to -i must be a hex ID number" << std::endl;
					return 1;
				}
			}
			break;

		case 'x':
			extract = true;
			break;

		case 's':
			stat = true;
			break;

		case 'v':
			if(logLevel < od::Logger::LOGLEVEL_DEBUG)
			{
				logLevel = static_cast<od::Logger::LogLevel>(1 + static_cast<int>(logLevel)); // i know, yucky enum abuse
			}
			break;

		case '?':
			if(optopt == 'i')
			{
				std::cerr << "Option -i requires a hex ID argument." << std::endl;

			}else
			{
				std::cout << "Unknown option -" << optopt << std::endl;
				printUsage();
			}
			return 1;
		}
	}

	if(optind >= argc)
	{
		std::cout << "Need at least a file argument." << std::endl;
	    printUsage();
	    return 1;
	}

	filename = std::string(argv[optind]);

	try
	{
		od::Logger::getDefaultLogger().setOutputLogLevel(logLevel);

		od::RiotFunctionLibrary &rfl = od::RiotFunctionLibrary::getSingleton();
		od::Logger::info() << "Got RFL " << rfl.getName() << " with " << rfl.getClassTypeCount() << " registered class types";

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

		od::SrscFile srscFile(filename);
		for(od::SrscFile::DirEntry entry : srscFile.getDirectory())
		{
			if(entry.type == 0x0102)
			{
			    od::DataReader dr(srscFile.getStreamForRecord(entry));

			    std::string classname;
			    dr >> classname;

			    dr.ignore(6);

			    uint16_t classType;
			    dr >> classType;

			    std::cout << classname << ": " << std::hex << classType << std::dec << std::endl;

			}
		}


	}catch(std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
