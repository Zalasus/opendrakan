/*
 * DataStream.cpp
 *
 *  Created on: 05.07.2014
 *      Author: Zalasus
 */

#include <odCore/DataStream.h>

#include <string>

#include <odCore/Exception.h>

namespace od
{

    DataReader::Ignore::Ignore(size_t n)
    : mCountByte(n)
    {
    }


    DataReader::DataReader()
    : mStream(nullptr)
    {
    }

	DataReader::DataReader(std::istream &stream)
	: mStream(&stream)
	{
		if(mStream == nullptr || !mStream->good())
		{
			throw IoException("Constructed DataReader with bad stream");
		}
	}

	DataReader::DataReader(const DataReader &dr)
	: mStream(dr.mStream)
	{
	}

	DataReader &DataReader::operator=(const DataReader &dr)
	{
	    mStream = dr.mStream;

	    return *this;
	}

	std::istream &DataReader::getStream()
	{
	    _checkStream();

	    return *mStream;
	}

	void DataReader::ignore(size_t n)
	{
        _checkStream();

        mStream->ignore(n);
        if(mStream->eof())
        {
            throw IoException("Unexpected EOF while ignoring characters");

        }else if(mStream->fail())
        {
             throw IoException("Failed to ignore characters");
        }
	}

	void DataReader::seek(size_t offset)
	{
	    _checkStream();

		mStream->seekg(offset);
	}

	size_t DataReader::tell()
	{
		_checkStream();

		return mStream->tellg();
	}

	void DataReader::read(char *data, size_t size)
	{
        _checkStream();

		mStream->read(data, size);

		if(mStream->eof())
		{
            throw IoException("Unexpected EOF while reading block of data");

        }else if(mStream->fail())
        {
		    throw IoException("Failed to read block of data");
		}
	}

    void DataReader::readTyped(const Ignore &i)
    {
        _checkStream();

	    this->ignore(i.getCount());
    }

    template <typename T>
    static void _readIntegral(DataReader &dr, T &value)
    {
        using ValType = typename std::remove_reference<T>::type;
        using UnsignedValType = typename std::make_unsigned<T>::type;

        // read into byte array first, then combine to value (only one IO call).
        //  using bitshifts makes this endianess-independent.
        uint8_t bytes[sizeof(ValType)];
        dr.read(reinterpret_cast<char*>(&bytes[0]), sizeof(ValType));

        ValType tempValue = 0;
        for(size_t i = 0; i < sizeof(ValType); ++i)
        {
            tempValue |= static_cast<UnsignedValType>(bytes[i]) << (i*8);
        }

        value = tempValue;
    }

	template <>
	void DataReader::readTyped<uint64_t>(uint64_t &v)
	{
		_readIntegral(*this, v);
	}

    template <>
	void DataReader::readTyped<uint32_t>(uint32_t &v)
    {
		_readIntegral(*this, v);
	}

    template <>
    void DataReader::readTyped<uint16_t>(uint16_t &v)
    {
		_readIntegral(*this, v);
	}

    template <>
	void DataReader::readTyped<uint8_t>(uint8_t &v)
    {
		_readIntegral(*this, v);
	}

	template <>
	void DataReader::readTyped<int64_t>(int64_t &v)
    {
		_readIntegral(*this, v);
	}

    template <>
	void DataReader::readTyped<int32_t>(int32_t &v)
    {
		_readIntegral(*this, v);
	}

    template <>
    void DataReader::readTyped<int16_t>(int16_t &v)
    {
		_readIntegral(*this, v);
	}

    template <>
	void DataReader::readTyped<int8_t>(int8_t &v)
    {
		this->read(reinterpret_cast<char*>(v), 1);
	}

    template <>
	void DataReader::readTyped<char>(char &v)
	{
		this->read(reinterpret_cast<char*>(v), 1);
	}

    template <>
	void DataReader::readTyped<float>(float &v)
	{
        // FIXME: host endianess dependent
		auto valuePtr = reinterpret_cast<char*>(&v);
        this->read(valuePtr, sizeof(v));
	}

    template <>
	void DataReader::readTyped<double>(double &v)
	{
        // FIXME: host endianess dependent
		auto valuePtr = reinterpret_cast<char*>(&v);
        this->read(valuePtr, sizeof(v));
	}

    template <>
	void DataReader::readTyped<std::string>(std::string &s)
	{
		uint16_t length;
		*this >> length;
		if(length == 0)
		{
			s.clear();
			return;
		}

		s.resize(length);
        this->read(&s[0], length);

        // since the padding is included in the length field, we have to
        //  manually trim it off if it exists
        if(s.back() == '\0')
        {
            s.resize(length-1);
        }
	}

    void DataReader::_checkStream()
    {
        if(mStream == nullptr)
	    {
	        throw Exception("Tried to use a DataReader without assigned stream");
	    }
    }


    DataWriter::DataWriter(std::ostream &out)
    : mStream(&out)
    {
    }

    template <typename T>
    static void _writeIntegral(DataWriter &dw, T value)
    {
        static_assert(std::is_integral<T>::value, "T must be integral");

        // turn value into byte array first, then write it out (only one IO call).
        //  using bitshifts makes this endianess-independent.
        uint8_t bytes[sizeof(T)];
        for(size_t i = 0; i < sizeof(T); ++i)
        {
            bytes[i] = (value >> i*8) & 0xff;
        }

        dw.write(reinterpret_cast<const char*>(&bytes[0]), sizeof(T));
    }

    // you figure out this SFINAE stuff. I'm done.
    template <>
    void DataWriter::writeTyped<uint8_t>(const uint8_t &v)
    {
        _writeIntegral(*this, v);
    }

    template <>
    void DataWriter::writeTyped<uint16_t>(const uint16_t &v)
    {
        _writeIntegral(*this, v);
    }

    template <>
    void DataWriter::writeTyped<uint32_t>(const uint32_t &v)
    {
        _writeIntegral(*this, v);
    }

    template <>
    void DataWriter::writeTyped<uint64_t>(const uint64_t &v)
    {
        _writeIntegral(*this, v);
    }

    template <>
    void DataWriter::writeTyped<int8_t>(const int8_t &v)
    {
        _writeIntegral(*this, v);
    }

    template <>
    void DataWriter::writeTyped<int16_t>(const int16_t &v)
    {
        _writeIntegral(*this, v);
    }

    template <>
    void DataWriter::writeTyped<int32_t>(const int32_t &v)
    {
        _writeIntegral(*this, v);
    }

    template <>
    void DataWriter::writeTyped<int64_t>(const int64_t &v)
    {
        _writeIntegral(*this, v);
    }

    template <>
    void DataWriter::writeTyped<float>(const float &v)
    {
        // FIXME: host-endianess-dependent
        auto valuePtr = reinterpret_cast<const char*>(&v);
        this->write(valuePtr, sizeof(v));
    }

    template <>
    void DataWriter::writeTyped<double>(const double &v)
    {
        // FIXME: host-endianess-dependent
        auto valuePtr = reinterpret_cast<const char*>(&v);
        this->write(valuePtr, sizeof(v));
    }

    void DataWriter::write(const char *data, size_t size)
    {
        if(mStream == nullptr) throw Exception("Invalid stream");

        mStream->write(data, size);
        if(mStream->fail())
        {
            throw IoException("Failed to write data block");
        }
    }

    std::streamoff DataWriter::tell()
    {
        return mStream->tellp();
    }

    void DataWriter::seek(std::streamoff off)
    {
        mStream->seekp(off);
    }


    MemoryInputBuffer::MemoryInputBuffer(const char *data, size_t size)
    {
        // for some reason, the stdlib can only use non-const pointers in streambufs.
        //  given that we never modify the buffer, using const_cast'ing the constness away should probably be okay.
        char *mutData = const_cast<char*>(data);
    	this->setg(mutData, mutData, mutData+size);
    }

    std::streampos MemoryInputBuffer::seekoff(std::streamoff off, std::ios_base::seekdir way, std::ios_base::openmode which)
    {
    	if(which != std::ios_base::in)
    	{
    		return -1;
    	}

    	if(way == std::ios_base::beg)
    	{
    		setg(eback(), eback() + off, egptr());

    	}else if(way == std::ios_base::end)
    	{
    		setg(eback(), egptr() + off, egptr());

    	}else if(way == std::ios_base::cur)
    	{
    		gbump(off);
    	}

    	return gptr() - eback();
    }

    std::streampos MemoryInputBuffer::seekpos(std::streampos sp, std::ios_base::openmode which)
    {
    	return seekoff(sp - pos_type(off_type(0)), std::ios_base::beg, which);
    }


    MemoryOutputBuffer::MemoryOutputBuffer(char *data, size_t size)
    {
        this->setp(data, data+size);
    }

    std::streampos MemoryOutputBuffer::seekoff(std::streamoff off, std::ios_base::seekdir way, std::ios_base::openmode which)
    {
        OD_UNIMPLEMENTED();
    }

    std::streampos MemoryOutputBuffer::seekpos(std::streampos sp, std::ios_base::openmode which)
    {
        return seekoff(sp - pos_type(off_type(0)), std::ios_base::beg, which);
    }


    VectorOutputBuffer::VectorOutputBuffer(std::vector<char> &v)
    : mVector(v)
    {
        this->setp(&mBackBuffer[0], &mBackBuffer[0] + BACKBUFFER_SIZE);
    }

    VectorOutputBuffer::~VectorOutputBuffer()
    {
        this->sync();
    }

    std::streambuf::int_type VectorOutputBuffer::overflow(std::streambuf::int_type ch)
    {
        int syncResult = this->sync();
        if(syncResult != 0)
        {
            return std::streambuf::traits_type::eof();
        }

        if(ch != std::streambuf::traits_type::eof())
        {
            return this->sputc(ch);

        }else
        {
            return std::streambuf::traits_type::eof() + 1; // standard says this has to be "not std::streambuf::traits_type::eof()" on success.
        }
    }

    std::streampos VectorOutputBuffer::seekoff(std::streamoff off, std::ios_base::seekdir way, std::ios_base::openmode which)
    {
        OD_UNIMPLEMENTED();
    }

    std::streampos VectorOutputBuffer::seekpos(std::streampos sp, std::ios_base::openmode which)
    {
        OD_UNIMPLEMENTED();
    }

    int VectorOutputBuffer::sync()
    {
        ptrdiff_t backbufferSize = this->pptr() - this->pbase();
        if(backbufferSize >= 0 && static_cast<size_t>(backbufferSize) <= BACKBUFFER_SIZE)
        {
            if(backbufferSize > 0)
            {
                mVector.insert(mVector.end(), mBackBuffer.begin(), mBackBuffer.begin() + backbufferSize);
            }

            this->setp(&mBackBuffer[0], &mBackBuffer[0] + BACKBUFFER_SIZE);

            return 0;

        }else
        {
            Logger::error() << "Bad pptr or pbase in VectorOutputBuffer";
            return -1;
        }
    }
}
