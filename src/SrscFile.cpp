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

struct membuf : std::streambuf
{
    membuf(char* begin, char* end) {
        this->setg(begin, begin, end);
    }
};

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
			throw std::runtime_error("Could not open SRSC file");
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

	SrscFile::DirEntry SrscFile::getDirectoryEntryByID(uint16_t id)
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

	SrscFile::RecordInfo SrscFile::getRecordInfo(const SrscFile::DirEntry &dirEntry)
	{
		RecordInfo info;
		DataReader in(mInputStream);
		mInputStream.seekg(dirEntry.dataOffset);

		if(dirEntry.type == 0x302) // seems to be a sound record
		{
			in >> info.name
			   >> info.flags
			   >> info.channels
			   >> info.bits
			   >> info.frequency
			   >> info.volume
			   >> info.dropoff
			   >> info.priority
			   >> info.decompressed_size
			   >> info.compression_rate
			   >> info.compressed_size;

		}else if((dirEntry.type & 0xff) == 0x01) // group declaration
		{

			in >> info.name;
			mInputStream.seekg(dirEntry.dataOffset);

		}else if(dirEntry.type == 0x200) // model name
		{
			in >> info.name;
			mInputStream.seekg(dirEntry.dataOffset);

		}else if(dirEntry.type == 0x0040) // texture/material
		{
			in >> info.width
			   >> info.height
			   >> info.row_spacing
			   >> info.bits_per_pixel
			   >> info.alpha_bits_per_pixel;

			in.ignore(2);

			in >> info.colour_key;

			in.read((char*)&info.dummy[0], 0x18);

			in >> info.compression_rate
			   >> info.compressed_size;
		}

		info.payloadOffset = mInputStream.tellg();
		info.payloadSize = dirEntry.dataSize - (info.payloadOffset - dirEntry.dataOffset);

		return info;
	}

	std::istream &SrscFile::getStreamForRecord(const SrscFile::DirEntry &dirEntry, bool decompress)
	{
		RecordInfo info = getRecordInfo(dirEntry);

		mInputStream.seekg(info.payloadOffset);

		/*if(!decompress)
		{
			return
		}
		mRecordPayloadBuffer = new char[info.payloadSize];
		mInputStream.read(payloadBuffer, info.payloadSize);

		membuf payloadStreamBuffer(payloadBuffer, payloadBuffer + info.payloadSize);

		zstr::istreambuf inflateBuffer(&payloadStreamBuffer, 1<<16, true);
		std::istream inflateStream(&inflateBuffer);
		inflateStream.exceptions(std::ios_base::badbit);

		const std::streamsize buff_size = 1 << 16;
		char *buff = new char [buff_size];
		while(true)
		{
			inflateStream.read(buff, buff_size);
			std::streamsize cnt = inflateStream.gcount();
			if (cnt == 0) break;
			out.write(buff, cnt);
		}
		delete[] buff;

*/
		return mInputStream;
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

			RecordInfo info = getRecordInfo(dirEntry);

			std::string filename;
			std::ostringstream ss;
			ss << prefix;
			if(info.name.empty())
			{
				ss << "record" << dirEntry.index << "-" << dirEntry.recordId << "-" << dirEntry.groupId;

			}else
			{
				ss << info.name;
			}
			ss << ".dat";

			std::ofstream out(ss.str(), std::ios::out | std::ios::binary);

			mInputStream.seekg(info.payloadOffset);

			char *payloadBuffer = new char[info.payloadSize];
			mInputStream.read(payloadBuffer, info.payloadSize);

			membuf payloadStreamBuffer(payloadBuffer, payloadBuffer + info.payloadSize);

			zstr::istreambuf inflateBuffer(&payloadStreamBuffer, 1<<16, true);
			std::istream inflateStream(&inflateBuffer);
			inflateStream.exceptions(std::ios_base::badbit);

			const std::streamsize buff_size = 1 << 16;
			char *buff = new char [buff_size];
			while(true)
			{
				inflateStream.read(buff, buff_size);
				std::streamsize cnt = inflateStream.gcount();
				if (cnt == 0) break;
				out.write(buff, cnt);
			}
			delete[] buff;

			out.flush();
			out.close();
		}
	}

	void SrscFile::_readHeaderAndDirectory()
	{
		DataReader in(mInputStream);

		uint32_t magic;
		in >> magic;
		if(magic != 0x43535253) // 'SRSC' in LE
		{
			throw std::runtime_error("Invalid magic number in SRSC file");
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





