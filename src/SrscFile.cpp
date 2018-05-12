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

	SrscFile::SrscFile(const FilePath &filePath)
	: mFilePath(filePath)
	{
		mInputStream.open(mFilePath.str().c_str(), std::ios::in | std::ios::binary);
		if(mInputStream.fail())
		{
			throw IoException("Could not open SRSC file");
		}

		_readHeaderAndDirectory();
	}

	SrscFile::~SrscFile()
	{
	}

	SrscFile::DirIterator SrscFile::getDirectoryBegin()
	{
		return mDirectory.begin();
	}

	SrscFile::DirIterator SrscFile::getDirectoryEnd()
	{
		return mDirectory.end();
	}

	SrscFile::DirIterator SrscFile::getDirIteratorById(RecordId id, DirIterator start)
	{
		while(start != mDirectory.end())
		{
			if(start->recordId == id)
			{
				return start;
			}

			++start;
		}

		return mDirectory.end();
	}

	SrscFile::DirIterator SrscFile::getDirIteratorByType(RecordType type, DirIterator start)
	{
		while(start != mDirectory.end())
		{
			if(start->type == type)
			{
				return start;
			}

			++start;
		}

		return mDirectory.end();
	}

	SrscFile::DirIterator SrscFile::getDirIteratorByTypeId(RecordType type, RecordId id, DirIterator start)
	{
		while(start != mDirectory.end())
		{
			if(start->recordId == id && start->type == type)
			{
				return start;
			}

			++start;
		}

		return mDirectory.end();
	}

	std::istream &SrscFile::getStreamForRecord(const SrscFile::DirEntry &dirEntry)
	{
		mInputStream.seekg(dirEntry.dataOffset);

		return mInputStream;
	}

	void SrscFile::decompressAll(const std::string &prefix, bool extractRaw)
	{
		for(DirEntry entry : mDirectory)
		{
			decompressRecord(prefix, entry, extractRaw);
		}
	}

	void SrscFile::decompressRecord(const std::string &prefix, const DirEntry &dirEntry, bool extractRaw)
	{
		if(extractRaw)
		{
			std::string filename;
			std::ostringstream ss;
			ss << prefix << "rawrecord" << dirEntry.index << "-" << std::hex << dirEntry.type << "-" << dirEntry.recordId << "-" << dirEntry.groupId << std::dec << ".dat";

			std::ofstream out(ss.str(), std::ios::out | std::ios::binary);

			mInputStream.seekg(dirEntry.dataOffset);

			for(size_t i = 0; i < dirEntry.dataSize; ++i)
			{
				out.put(mInputStream.get());
			}

			out.close();

		}else
		{
		    throw UnsupportedException("Can't decompress right now");
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
		if(mVersion != 0x100 && mVersion != 0x101)
		{
			throw UnsupportedException("Unsupported SRSC version");
		}

		in >> mDirectoryOffset;

		uint16_t recordCount;
		in >> recordCount;

		mDirectory.resize(recordCount);

		mInputStream.seekg(mDirectoryOffset);

		for(size_t i = 0; i < recordCount; ++i)
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
