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
#include <mutex>

#include <odCore/FilePath.h>
#include <odCore/DataStream.h>
#include <odCore/SrscRecordTypes.h>

namespace od
{

    typedef uint16_t RecordId;
    typedef uint16_t RecordType;

    class RecordOutputCursor
    {
    public:

        void flush();

    };

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

		class RecordInputCursor
        {
        public:

            RecordInputCursor(SrscFile &file, std::mutex &mutex, DirIterator &dirIt);
            RecordInputCursor(RecordInputCursor &&c);

            inline const DirIterator &getDirIterator() { return mDirIterator; }

            DataReader getReader();

            bool isValid();

            bool next();
            bool nextOfType(RecordType type, int32_t maxDistance = -1);
            bool nextOfId(RecordId id, int32_t maxDistance = -1);
            bool nextOfTypeId(RecordType type, RecordId id, int32_t maxDistance = -1);

            inline bool nextOfType(SrscRecordType type, int32_t maxDistance = -1) { return nextOfType(static_cast<RecordType>(type), maxDistance); }
            inline bool nextOfTypeId(SrscRecordType type, RecordId id, int32_t maxDistance = -1) { return nextOfTypeId(static_cast<RecordType>(type), id, maxDistance); }

            void moveTo(const DirIterator &dirIt);


        private:

            SrscFile &mFile;
            DirIterator mDirIterator;
            std::unique_lock<std::mutex> mLock;
        };

		SrscFile(const FilePath &filePath);
		~SrscFile();

		inline const FilePath &getFilePath() const { return mFilePath; }
		inline uint16_t getVersion() const { return mVersion; };
		inline size_t getRecordCount() const { return mDirectory.size(); };
		inline const std::vector<DirEntry> &getDirectory() const { return mDirectory; };

		// low level access to directory. no locking
		DirIterator getDirectoryBegin();
		DirIterator getDirectoryEnd();
		std::istream &getStreamForRecord(const DirIterator &dirIt);

		// high level interface with proper locking
        RecordInputCursor getFirstRecordOfType(RecordType type);
        RecordInputCursor getFirstRecordOfId(RecordId id);
		RecordInputCursor getFirstRecordOfTypeId(RecordType type, RecordId id);

		// convenience
		inline RecordInputCursor getFirstRecordOfType(SrscRecordType type) { return getFirstRecordOfType(static_cast<RecordType>(type)); }
        inline RecordInputCursor getFirstRecordOfTypeId(SrscRecordType type, RecordId id) { return getFirstRecordOfTypeId(static_cast<RecordType>(type), id); }

        // "historic reasons"
		void decompressAll(const od::FilePath &outputDir, bool extractRaw);
        void decompressRecord(const od::FilePath &outputDir, const DirIterator &dirIt, bool extractRaw);


	protected:

		void _readHeaderAndDirectory();
		void _checkDirIterator(const DirIterator &it);

		FilePath mFilePath;
		std::ifstream mInputStream;

		uint16_t mVersion;
		uint32_t mDirectoryOffset;
		std::vector<DirEntry> mDirectory;

		std::mutex mMutex;
	};

}

#endif /* INCLUDE_SRSCFILE_H_ */
