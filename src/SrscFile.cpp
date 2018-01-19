/*
 * SrscFile.cpp
 *
 *  Created on: 30 Dec 2017
 *      Author: zal
 */

#include "SrscFile.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <streambuf>

#include "DataStream.h"
#include "Exception.h"

namespace od
{

	SrscFile::SrscFile(FilePath filePath)
	: mFilePath(filePath)
	, mRecordPayloadBuffer(nullptr)
	{
		mInputStream.open(mFilePath.str().c_str(), std::ios::in | std::ios::binary);
		if(mInputStream.fail())
		{
			// huh, too bad
			throw Exception("Could not open SRSC file");
		}

		_readHeaderAndDirectory();
	}

	SrscFile::~SrscFile()
	{
		if(mInputStream.is_open())
		{
			mInputStream.close();
		}

		if(mRecordPayloadBuffer != nullptr)
		{
			delete[] mRecordPayloadBuffer;
		}
	}

	uint8_t SrscFile::getVersion()
	{
		return mVersion;
	}

	size_t SrscFile::getRecordCount()
	{
		return mRecordCount;
	}

	std::vector<SrscFile::DirEntry>::iterator SrscFile::getDirectoryBegin()
	{
		return mDirectory.begin();
	}

	std::vector<SrscFile::DirEntry>::iterator SrscFile::getDirectoryEnd()
	{
		return mDirectory.end();
	}

	SrscFile::DirEntry SrscFile::getDirectoryEntryByID(RecordId id)
	{
		for(DirEntry entry : mDirectory)
		{
			if(entry.recordId == id)
			{
				return entry;
			}
		}

		throw std::runtime_error("Record ID not found in directory");
	}

	SrscFile::DirEntry SrscFile::getDirectoryEntryByTypeAndID(RecordType type, RecordId id)
    {
        for(DirEntry entry : mDirectory)
        {
            if(entry.recordId == id && entry.type == type)
            {
                return entry;
            }
        }

        throw std::runtime_error("Record ID and type not found in directory");
    }

	std::istream &SrscFile::getStreamForRecord(const SrscFile::DirEntry &dirEntry)
	{
		mInputStream.seekg(dirEntry.dataOffset);

		return mInputStream;
	}

	std::istream &SrscFile::getStreamForRecordTypeId(RecordType type, RecordId id)
    {
	    return getStreamForRecord(getDirectoryEntryByTypeAndID(type, id));
    }

	void SrscFile::decompressAll(const std::string &prefix, bool extractRaw)
	{
		auto it = getDirectoryBegin();
		while(it != getDirectoryEnd())
		{
			decompressRecord(prefix, *it, extractRaw);

			++it;
		}
	}

	void SrscFile::decompressRecord(const std::string &prefix, DirEntry dirEntry, bool extractRaw)
	{
		if(extractRaw)
		{
			std::string filename;
			std::ostringstream ss;
			ss << prefix << "rawrecord" << dirEntry.index << "-" << std::hex << dirEntry.recordId << "-" << dirEntry.groupId << std::dec << ".dat";

			std::ofstream out(ss.str(), std::ios::out | std::ios::binary);

			mInputStream.seekg(dirEntry.dataOffset);

			for(size_t i = 0; i < dirEntry.dataSize; ++i)
			{
				out.put(mInputStream.get());
			}

			out.close();

		}else
		{
		    throw Exception("Can't decompress right now");
		}
	}

	void SrscFile::_readHeaderAndDirectory()
	{
		DataReader in(mInputStream);

		uint32_t magic;
		in >> magic;
		if(magic != 0x43535253) // 'SRSC' in LE
		{
			throw Exception("Invalid magic number in SRSC file");
		}

		in >> mVersion;
		in >> mDirectoryOffset;
		in >> mRecordCount;

		mDirectory.resize(mRecordCount);

		mInputStream.seekg(mDirectoryOffset);

		for(size_t i = 0; i < mRecordCount; ++i)
		{
			DirEntry entry;
			in >> entry.type
			   >> entry.recordId
			   >> entry.groupId
			   >> entry.dataOffset
			   >> entry.dataSize;

			entry.index = i;

			mDirectory[i] = entry;
		}
	}
}





