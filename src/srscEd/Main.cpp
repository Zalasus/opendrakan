/*
 * Main.cpp
 *
 *  Created on: 24 Dec 2018
 *      Author: zal
 */

#include <iostream>
#include <iomanip>
#include <unistd.h>

#include <odCore/SrscFile.h>
#include <odCore/FilePath.h>

static void srscStat(od::SrscFile &file, size_t dataByteCount)
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

    auto begin = file.getDirectoryBegin();
    auto it = begin;
    while(it != file.getDirectoryEnd())
    {

        std::cout
            << std::setw(6) << (it - begin)
            << std::setw(6) << std::hex << it->type << std::dec
            << std::setw(8) << it->dataSize
            << std::setw(6) << std::hex << it->recordId << std::dec
            << std::setw(6) << std::hex << it->groupId << std::dec;

        std::cout << "  ";

        od::DataReader dr(file.getStreamForRecord(it));

        for(size_t i = 0; i < it->dataSize; ++i)
        {
            if(i >= dataByteCount)
            {
                std::cout << "[...]";
                break;
            }

            uint8_t b;
            dr >> b;

            printf("%02x ", b);
        }

        std::cout << std::endl;

        ++it;
    }
}

static void printUsage()
{
    std::cout
        << "Usage: " << std::endl << std::endl
        << "    srsced [options] [file]" << std::endl << std::endl
        << "Reads and edits SRSC files (editing unimplemented)" << std::endl << std::endl
        << "Options:" << std::endl
        << "    -o <path>  Output directory (default '.')" << std::endl
        << "    -x         Extract raw records" << std::endl
        << "    -i <id>    Limit extraction to records with ID <id>" << std::endl
        << "    -c <count> Sets count of data bytes to print (default 10)" << std::endl
        << "    -h         Display this message and exit" << std::endl << std::endl
        << "If no option is given, the file's directory is printed" << std::endl
        << std::endl;
}


int main(int argc, char **argv)
{
    std::string filename;
    od::FilePath outputPath(".");
    bool extract = false;
    bool limitExtractionIds = false;
    uint16_t extractRecordId = 0;
    size_t dataByteCount = 10;
    int c;
    while((c = getopt(argc, argv, "i:o:xthc:")) != -1)
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
                limitExtractionIds = true;
            }
            break;

        case 'c':
            {
                std::istringstream iss(optarg);
                iss >> dataByteCount;
                if(iss.fail())
                {
                    std::cout << "Argument to -c must be a decimal byte count" << std::endl;
                    return 1;
                }
            }
            break;


        case 'x':
            extract = true;
            break;

        case 'o':
            outputPath = od::FilePath(optarg);
            break;

        case 'h':
            printUsage();
            return 0;

        case '?':
            if(optopt == 'i')
            {
                std::cerr << "Option -i requires a hex ID argument." << std::endl;

            }else if(optopt == 'o')
            {
                std::cerr << "Option -o requires a valid path argument" << std::endl;

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
        od::SrscFile srscFile(filename);

        if(extract)
        {
            if(limitExtractionIds)
            {
                std::cout << "Extracting all records with ID " << std::hex << extractRecordId << std::dec << " to " << outputPath << std::endl;

                auto cursor = srscFile.getFirstRecordOfId(extractRecordId);
                while(cursor.isValid())
                {
                    srscFile.decompressRecord(outputPath, cursor.getDirIterator(), true);
                    cursor.nextOfId(extractRecordId);
                }

            }else
            {
                std::cout << "Extracting all records to " << outputPath << std::endl;

                srscFile.decompressAll(outputPath, true);
            }

        }else
        {
            srscStat(srscFile, dataByteCount);
        }


    }catch(std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
