/*
 * SrscFile.h
 *
 *  Created on: 30 Dec 2017
 *      Author: zal
 */

#ifndef INCLUDE_SRSCFILE_H_
#define INCLUDE_SRSCFILE_H_

#include <string>
#include <vector>
#include <fstream>

#include "zstr.hpp"
#include "FilePath.h"
#include "DataStream.h"

namespace od
{

	class SrscFile
	{
	public:

		struct DirEntry
		{
			uint16_t index;
			uint16_t type;
			uint16_t recordId;
			uint16_t groupId;
			uint32_t dataOffset;
			uint32_t dataSize;
		};

		struct RecordInfo
		{
			uint32_t payloadOffset;
			uint32_t payloadSize;

			std::string name;
			uint32_t flags;
			uint16_t channels;
			uint16_t bits;
			uint32_t frequency;
			uint32_t volume;
			float dropoff;
			uint32_t priority;
			uint32_t decompressed_size;
			uint32_t compression_rate;
			uint32_t compressed_size;

			uint32_t width;
			uint32_t height;
			uint32_t row_spacing;
			uint16_t bits_per_pixel;
			uint16_t alpha_bits_per_pixel;
			uint32_t colour_key;



			uint8_t dummy[0x18];
		};

		SrscFile(FilePath filePath);
		~SrscFile();

		uint8_t getVersion();
		size_t getRecordCount();

		std::vector<DirEntry>::iterator getDirectoryBegin();
		std::vector<DirEntry>::iterator getDirectoryEnd();
		DirEntry getDirectoryEntryByID(uint16_t id);

		RecordInfo getRecordInfo(const DirEntry &dirEntry);

		std::istream &getStreamForRecord(const DirEntry &dirEntry, bool decompress = true);

		void decompressAll(const std::string &prefix, bool extractRaw);
		void decompressRecord(const std::string &prefix, DirEntry dirEntry, bool extractRaw);


	protected:

		void _readHeaderAndDirectory();

		FilePath mFilePath;
		std::ifstream mInputStream;

		uint16_t mVersion;
		uint32_t mDirectoryOffset;
		uint16_t mRecordCount;
		std::vector<DirEntry> mDirectory;

		char *mRecordPayloadBuffer;
	};

}

#endif /* INCLUDE_SRSCFILE_H_ */
