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
	bool extract = false;
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
		od::Logger::getDefaultLogger().setPrintDebug(true);

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

		od::DataReader dr(srscFile.getStreamForRecordTypeId(0x0001, 0));

		uint32_t layerCount;
		uint32_t dummy;

		uint32_t      width; // in vertices; a layer with widthand height of 1 is a square with 1 vertex in each corner
        uint32_t      height;
        uint32_t      type;  // 0 = floor, 1 = ceiling, 2 = between
        uint32_t    	origin_x;
        uint32_t	origin_y;
        float       world_height;
        std::string      layer_name;
        uint32_t      flags; // 2 = member of alternate blending group
        float       light_direction;
        float       light_ascension;
        uint32_t     light_color;
        uint32_t     ambient_color;
        uint32_t      light_dropoff_type; // 0 = none; 1 = from north to south; 2 = E->W; 3 = S->N; 4 = W->E
        uint32_t      dummy1;
        uint32_t      dummy2;

        dr  >> layerCount
			>> dummy;

        for(size_t i = 0; i < layerCount; ++i)
        {
			dr  >> width
				>> height
				>> type
				>> origin_x
				>> origin_y
				>> world_height
				>> layer_name
				>> flags
				>> light_direction
				>> light_ascension
				>> light_color
				>> ambient_color
				>> light_dropoff_type
				>> dummy1
				>> dummy2;

			std::cout << "Layer name " << layer_name << std::endl;
			std::cout << " D1 " <<  std::hex << dummy1 << std::dec << std::endl;
			std::cout << " D2 " <<  std::hex << dummy2 << std::dec << std::endl;
        }

        dr >> dummy1
		   >> dummy2;

        std::cout << "---------" << std::endl;
        std::cout << "D1 " <<  std::hex << dummy1 << std::dec << std::endl;
        std::cout << "D2 " <<  std::hex << dummy1 << std::dec << std::endl;

		od::ZStream in(dr.getStream());
		std::ofstream out("out/layerout.dat", std::ios::out | std::ios::binary);
		size_t n = 0;

		int c = in.get();
		while(c != od::ZStream::traits_type::eof())
		{
			c = in.get();
			out.put(c);
			n++;
		}

		std::cout << "Decompressed " << n << " bytes to out/layerout.dat" << std::endl;

	}catch(std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
