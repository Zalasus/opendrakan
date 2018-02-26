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
#include "SrscRecordTypes.h"

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

		typedef std::vector<DirEntry>::iterator DirIterator;

		SrscFile(const FilePath &filePath);
		~SrscFile();

		inline uint16_t getVersion() const { return mVersion; };
		inline size_t getRecordCount() const { return mDirectory.size(); };
		inline const std::vector<DirEntry> &getDirectory() const { return mDirectory; };


		DirIterator getDirectoryBegin();
		DirIterator getDirectoryEnd();

		// FIXME: Holy shit, what happened here? Get rid of these methods! Only the first three should suffice
		DirIterator getDirIteratorById(RecordId id, DirIterator start);
		DirIterator getDirIteratorByType(RecordType type, DirIterator start);
		DirIterator getDirIteratorByTypeId(RecordType type, RecordId id, DirIterator start);
		inline DirIterator getDirIteratorByType(SrscRecordType type, DirIterator start) { return getDirIteratorByType(static_cast<RecordType>(type), start); }
		inline DirIterator getDirIteratorByTypeId(SrscRecordType type, RecordId id, DirIterator start) { return getDirIteratorByTypeId(static_cast<RecordType>(type), id, start); }
		inline DirIterator getDirIteratorById(RecordId id) { return getDirIteratorById(id, mDirectory.begin()); }
		inline DirIterator getDirIteratorByType(RecordType type) { return getDirIteratorByType(type, mDirectory.begin()); }
		inline DirIterator getDirIteratorByTypeId(RecordType type, RecordId id) { return getDirIteratorByTypeId(type, id, mDirectory.begin()); }
		inline DirIterator getDirIteratorByType(SrscRecordType type) { return getDirIteratorByType(static_cast<RecordType>(type), mDirectory.begin()); }
		inline DirIterator getDirIteratorByTypeId(SrscRecordType type, RecordId id) { return getDirIteratorByTypeId(static_cast<RecordType>(type), id, mDirectory.begin()); }

		std::istream &getStreamForRecord(const DirEntry &dirEntry);
		inline std::istream &getStreamForRecord(const DirIterator &dirIt) { return getStreamForRecord(*dirIt); }
		inline std::istream &getStreamForRecordType(RecordType type) { return getStreamForRecord(getDirIteratorByType(type)); }
		inline std::istream &getStreamForRecordTypeId(RecordType type, RecordId id) { return getStreamForRecord(getDirIteratorByTypeId(type, id)); }
		inline std::istream &getStreamForRecordType(SrscRecordType type) { return getStreamForRecord(getDirIteratorByType(static_cast<RecordType>(type))); }
		inline std::istream &getStreamForRecordTypeId(SrscRecordType type, RecordId id) { return getStreamForRecord(getDirIteratorByTypeId(static_cast<RecordType>(type), id)); }

		void decompressAll(const std::string &prefix, bool extractRaw);
		void decompressRecord(const std::string &prefix, const DirEntry &dirEntry, bool extractRaw);
		inline void decompressRecord(const std::string &prefix, const DirIterator &dirIt, bool extractRaw) { decompressRecord(prefix, *dirIt, extractRaw); }


	protected:

		void _readHeaderAndDirectory();

		FilePath mFilePath;
		std::ifstream mInputStream;

		uint16_t mVersion;
		uint32_t mDirectoryOffset;
		std::vector<DirEntry> mDirectory;
	};

}

#endif /* INCLUDE_SRSCFILE_H_ */
