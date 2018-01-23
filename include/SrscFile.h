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

#include "FilePath.h"
#include "DataStream.h"

namespace od
{

    typedef uint16_t RecordId;
    typedef uint16_t RecordType;

	class SrscFile
	{
	public:

		struct DirEntry
		{
			uint16_t index;
			RecordType type;
			RecordId recordId;
			RecordId groupId;
			uint32_t dataOffset;
			uint32_t dataSize;
		};

		SrscFile(FilePath filePath);
		~SrscFile();

		uint16_t getVersion();
		size_t getRecordCount();

		std::vector<DirEntry>::iterator getDirectoryBegin();
		std::vector<DirEntry>::iterator getDirectoryEnd();
		DirEntry getDirectoryEntryByID(RecordId id);
		DirEntry getDirectoryEntryByTypeAndID(RecordType type, RecordId id);

		std::istream &getStreamForRecord(const DirEntry &dirEntry);
		std::istream &getStreamForRecordTypeId(RecordType type, RecordId id);

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
