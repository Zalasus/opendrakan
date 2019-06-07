/*
 * SrscFile.cpp
 *
 *  Created on: 30 Dec 2017
 *      Author: zal
 */

#include <odCore/SrscFile.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <streambuf>
#include <functional>
#include <algorithm>

#include <odCore/DataStream.h>
#include <odCore/Exception.h>

namespace od
{

    static bool _stepForward(SrscFile::DirIterator &it, const SrscFile::DirIterator &end, int32_t maxSteps, const std::function<bool(const SrscFile::DirEntry&)> &predicate)
    {
        if(it == end)
        {
            return false;
        }

        int32_t step = 0;
        while(++it != end && ++step <= maxSteps)
        {
            if(predicate(*it))
            {
                return true;
            }
        }

        if(step >= maxSteps)
        {
            it = end;
        }

        return false;
    }

    SrscFile::RecordInputCursor::RecordInputCursor(SrscFile &file, std::mutex &mutex, DirIterator &dirIt)
    : mFile(file)
    , mDirIterator(dirIt)
    , mLock(mutex, std::try_to_lock)
    {
        if(!mLock.owns_lock())
        {
            throw od::Exception("Created input cursor for SrscFile when file was still locked (probably by another cursor)");
        }
    }

    SrscFile::RecordInputCursor::RecordInputCursor(RecordInputCursor &&c)
    : mFile(c.mFile)
    , mDirIterator(c.mDirIterator)
    , mLock(std::move(c.mLock))
    {
    }

    DataReader SrscFile::RecordInputCursor::getReader()
    {
        if(!isValid())
        {
            throw od::Exception("Tried to access record using invalid cursor");
        }

        //size_t availableBytes = mDirIterator->dataSize;
        //return DataReader(mFile.getStreamForRecord(*mDirIterator), availableBytes);

        return DataReader(mFile.getStreamForRecord(mDirIterator));
    }

    bool SrscFile::RecordInputCursor::isValid()
    {
        return (mDirIterator != mFile.getDirectoryEnd()) && mLock.owns_lock();
    }

    bool SrscFile::RecordInputCursor::next()
    {
        if(mDirIterator != mFile.getDirectoryEnd())
        {
            ++mDirIterator;
        }

        return mDirIterator != mFile.getDirectoryEnd();
    }

    bool SrscFile::RecordInputCursor::nextOfType(RecordType type, int32_t maxDistance)
    {
        auto pred = [type](const SrscFile::DirEntry &d) { return d.type == type; };
        return _stepForward(mDirIterator, mFile.getDirectoryEnd(), maxDistance, pred);
    }

    bool SrscFile::RecordInputCursor::nextOfId(RecordId id, int32_t maxDistance)
    {
        auto pred = [id](const SrscFile::DirEntry &d) { return d.recordId == id; };
        return _stepForward(mDirIterator, mFile.getDirectoryEnd(), maxDistance, pred);
    }

    bool SrscFile::RecordInputCursor::nextOfTypeId(RecordType type, RecordId id, int32_t maxDistance)
    {
        auto pred = [type, id](const SrscFile::DirEntry &d) { return d.type == type && d.recordId == id; };
        return _stepForward(mDirIterator, mFile.getDirectoryEnd(), maxDistance, pred);
    }

    void SrscFile::RecordInputCursor::moveTo(const SrscFile::DirIterator &dirIt)
    {
        mFile._checkDirIterator(dirIt);

        mDirIterator = dirIt;
    }


	SrscFile::SrscFile(const FilePath &filePath)
	: mFilePath(filePath)
	{
		mInputStream.open(mFilePath.str().c_str(), std::ios::in | std::ios::binary);
		if(mInputStream.fail())
		{
			throw IoException("Could not open SRSC file '" + mFilePath.str() + "'");
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

	std::istream &SrscFile::getStreamForRecord(const SrscFile::DirIterator &dirIt)
	{
	    _checkDirIterator(dirIt);

		mInputStream.seekg(dirIt->dataOffset);

		return mInputStream;
	}

	SrscFile::RecordInputCursor SrscFile::getFirstRecordOfType(RecordType type)
	{
	    auto pred = [type](const SrscFile::DirEntry &d) { return d.type == type; }; // TODO: duplicate predicate (see RecordInputCursor)
	    auto it = std::find_if(getDirectoryBegin(), getDirectoryEnd(), pred);
	    return RecordInputCursor(*this, mMutex, it);
	}

    SrscFile::RecordInputCursor SrscFile::getFirstRecordOfId(RecordId id)
    {
        // TODO: we could create a map indexing the first record of every ID, thus speeding up finding assets in a file immensely
        //  or maybe let the asset containers do that. IDK

        auto pred = [id](const SrscFile::DirEntry &d) { return d.recordId == id; };
        auto it = std::find_if(getDirectoryBegin(), getDirectoryEnd(), pred);
        return RecordInputCursor(*this, mMutex, it);
    }

    SrscFile::RecordInputCursor SrscFile::getFirstRecordOfTypeId(RecordType type, RecordId id)
    {
        auto pred = [type, id](const SrscFile::DirEntry &d) { return d.type == type && d.recordId == id; };
        auto it = std::find_if(getDirectoryBegin(), getDirectoryEnd(), pred);
        return RecordInputCursor(*this, mMutex, it);
    }

	void SrscFile::decompressAll(const std::string &prefix, bool extractRaw)
	{
		for(auto it = mDirectory.begin(); it != mDirectory.end(); ++it)
		{
			decompressRecord(prefix, it, extractRaw);
		}
	}

	void SrscFile::decompressRecord(const std::string &prefix, const DirIterator &dirIt, bool extractRaw)
	{
	    _checkDirIterator(dirIt);

	    const DirEntry &dirEntry = *dirIt;

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
		    throw UnsupportedException("The 'decompress' feature in SrscFile is historic and no longer supported. Use odDb to access decompressed record data");
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

    void SrscFile::_checkDirIterator(const DirIterator &dirIt)
    {
        if(dirIt == getDirectoryEnd())
        {
            throw od::Exception("Tried to use directory iterator not within directory bounds");
        }

        // ugly hack: DirIterator is a typedef of std::vector's iterator, but that gives us no proper
        // way of checking whether the iterator was created by our mDirectory. we could create our own
        // iterator containing a ref to it's parent, but for now, check if the adress is within range
        DirEntry *entry = &(*dirIt);
        if(entry < mDirectory.data() || entry > (mDirectory.data() + mDirectory.size()))
        {
            throw od::Exception("Tried use directory iterator that does not belong to this SrscFile instance");
        }
    }
}
