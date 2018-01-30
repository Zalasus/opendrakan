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

		SrscFile(const FilePath &filePath);
		~SrscFile();

		inline uint16_t getVersion() const { return mVersion; };
		inline size_t getRecordCount() const { return mDirectory.size(); };
		inline const std::vector<DirEntry> &getDirectory() const { return mDirectory; };

		DirEntry getDirectoryEntryByID(RecordId id);
		DirEntry getDirectoryEntryByTypeAndID(RecordType type, RecordId id);

		std::istream &getStreamForRecord(const DirEntry &dirEntry);
		std::istream &getStreamForRecordTypeId(RecordType type, RecordId id);

		void decompressAll(const std::string &prefix, bool extractRaw);
		void decompressRecord(const std::string &prefix, const DirEntry &dirEntry, bool extractRaw);


	protected:

		void _readHeaderAndDirectory();

		FilePath mFilePath;
		std::ifstream mInputStream;

		uint16_t mVersion;
		uint32_t mDirectoryOffset;
		std::vector<DirEntry> mDirectory;

		char *mRecordPayloadBuffer;
	};

}

#endif /* INCLUDE_SRSCFILE_H_ */
