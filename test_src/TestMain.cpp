/*
 * TestMain.cpp
 *
 *  Created on: 30 Dec 2017
 *      Author: zal
 */

#include <iostream>
#include <iomanip>

#include "SrscFile.h"


void statTextures(od::SrscFile &file)
{
	std::cout << "Got SRSC version " << (int32_t)file.getVersion() << " with " << file.getRecordCount() << " records." << std::endl;
	std::cout << "Records:" << std::endl;

	std::cout << std::setw(6) << "Index"
			  << std::setw(6) << "Type"
			  << std::setw(8) << "Size"
			  << std::setw(6) << "RecID"
			  << std::setw(6) << "GruID"
			  << std::setw(8) << "Width"
			  << std::setw(8) << "Heigth"
			  << std::setw(8) << "BperRow"
			  << std::setw(6) << "BPP"
			  << std::setw(8) << "ABPP"
			  << std::setw(9) << "ColorKey"
			  << std::setw(8) << "ComRate"
			  << std::setw(8) << "ComSize"
			  << std::setw(8) << "???"
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

		if(it->type == 0x0040) // texture
		{
			std::cout
				<< std::setw(8) << info.width
				<< std::setw(8) << info.height
				<< std::setw(8) << info.row_spacing
				<< std::setw(6) << info.bits_per_pixel
				<< std::setw(8) << info.alpha_bits_per_pixel
				<< std::setw(9) << std::hex << info.colour_key << std::dec
				<< std::setw(8) << info.compression_rate
				<< std::setw(8) << info.compressed_size;

			for(size_t i = 0; i < 0x18; ++i)
			{
				printf(" %02x", info.dummy[i]);
			}

			if(info.compressed_size != info.payloadSize)
			{
				std::cout << "  BAD COMSIZE!!";
			}

			if(info.row_spacing != info.width*(info.bits_per_pixel/8))
			{
				std::cout << "  BAD ROW SPACING!!";
			}
		}

		std::cout << std::endl;

		++it;
	}
}

void srscStat(od::SrscFile &file)
{
	std::cout << "Got SRSC version " << (int32_t)file.getVersion() << " with " << file.getRecordCount() << " records." << std::endl;
	std::cout << "Records:" << std::endl;

	std::cout << std::setw(6) << "Index"
			  << std::setw(6) << "Type"
			  << std::setw(8) << "Size"
			  << std::setw(6) << "RecID"
			  << std::setw(6) << "GruID"
			  << std::setw(20) << "Name"
			  << std::setw(8) << "Flags"
			  << std::setw(6) << "Chan"
			  << std::setw(6) << "Bits"
			  << std::setw(8) << "Freq"
			  << std::setw(9) << "Volume"
			  << std::setw(8) << "Dropoff"
			  << std::setw(8) << "Prio"
			  << std::setw(8) << "ComSize"
			  << std::setw(8) << "ComRate"
			  << std::setw(8) << "UCmSize"
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

		if(it->type == 0x302) // sound record
		{
			std::cout << std::setw(20) << info.name
					  << std::setw(8) << info.flags
					  << std::setw(6) << info.channels
					  << std::setw(6) << info.bits
					  << std::setw(8) << info.frequency
					  << std::setw(9) << std::hex << info.volume << std::dec
					  << std::setw(8) << info.dropoff
					  << std::setw(8) << info.priority
					  << std::setw(8) << info.decompressed_size
					  << std::setw(8) << info.compression_rate;

			if(info.compression_rate == 0)
			{
				std::cout<< std::setw(8) << "N/A";

			}else
			{
				std::cout<< std::setw(8) << info.compressed_size;
				if(info.compressed_size != info.payloadSize)
				{
					std::cout << "  BAD COMSIZE!!";
				}
			}


		}else if((it->type & 0xff) == 0x01) // group definitions seem to always have low byte 0x01
		{
			std::cout << std::setw(20) << info.name;

		}else if(it->type == 0x200) // model name
		{
			std::cout << std::setw(20) << info.name;
		}

		std::cout << std::endl;

		++it;
	}
}

void printUsage()
{
	std::cout
		<< "Usage: opendrakan [options] filename" << std::endl
		<< "Options:" << std::endl
		<< "    -x         Extract" << std::endl
		<< "    -r         Extract raw (when -x)" << std::endl
		<< "    -i <id>    Extract only record with ID <id>" << std::endl
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
	bool extractRaw = false;
	bool statTex = false;
	uint16_t extractRecordId = 0;
	for(int i = 1; i < argc; ++i)
	{
		std::string arg(argv[i]);

		if(arg == "-x")
		{
			extract = true;

		}else if(arg == "-i")
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

		}else if(arg == "-r")
		{
			extractRaw = true;

		}else if(arg == "-t")
		{
			statTex = true;

		}else
		{
			filename = arg;
		}
	}


	try
	{
		od::SrscFile file(filename);

		statTex ? statTextures(file) : srscStat(file);

		if(extract)
		{
			if(extractRecordId == 0)
			{
				if(extractRaw)
				{
					std::cout << "Extracting all to out/" << std::endl;

				}else
				{
					std::cout << "Decompressing all to out/" << std::endl;

				}

				file.decompressAll("out/", extractRaw);

			}else
			{
				od::SrscFile::DirEntry dirEntry = file.getDirectoryEntryByID(extractRecordId);

				if(extractRaw)
				{
					std::cout << "Extracting record with ID " << std::hex << extractRecordId << std::dec << " to out/" << std::endl;

				}else
				{
					std::cout << "Decompressing record with ID " << std::hex << extractRecordId << std::dec << " to out/" << std::endl;
				}

				file.decompressRecord("out/", dirEntry, extractRaw);
			}
		}

	}catch(std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
