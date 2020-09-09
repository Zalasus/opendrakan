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
	    if(mStream == nullptr)
	    {
	        throw Exception("Called getStream() on DataReaer without assigned stream");
	    }

	    return *mStream;
	}

	void DataReader::ignore(size_t n)
	{
		for(size_t i = 0; i < n; ++i)
		{
			_getNext();
		}
	}

	void DataReader::seek(size_t offset)
	{
	    if(mStream == nullptr)
	    {
	        throw Exception("Called seek() on DataReader without assigned stream");
	    }

		mStream->seekg(offset);
	}

	size_t DataReader::tell()
	{
		if(mStream == nullptr)
        {
            throw Exception("Called tell() on DataReader without assigned stream");
        }

		return mStream->tellg();
	}

	void DataReader::read(char *data, size_t size)
	{
	    if(mStream == nullptr)
	    {
	        throw Exception("Called read() on a DataReader without assigned stream");
	    }

		mStream->read(data, size);

		size_t charsRead = mStream->gcount();
		if(charsRead != size)
		{
		    throw IoException("DataReader encountered unexpected EOF while reading block of data");
		}
	}

	uint8_t DataReader::_getNext()
	{
	    if(mStream == nullptr)
	    {
	        throw IoException("Tried to read from DataReader without assigned stream");
	    }

		int c = mStream->get();

		if(c == std::istream::traits_type::eof())
		{
			throw IoException("DataReader encountered unexpected EOF");
		}

		return c;
	}

    DataReader &DataReader::operator >> (const DataReader::Ignore &i)
    {
	    this->ignore(i.getCount());

        return *this;
    }

	template <>
	DataReader &DataReader::operator >> <uint64_t>(uint64_t &l)
	{
		_stupidlyReadIntegral<uint64_t>(l);

		return *this;
	}

    template <>
	DataReader &DataReader::operator >> <uint32_t>(uint32_t &i)
	{
		_stupidlyReadIntegral<uint32_t>(i);

		return *this;
	}

    template <>
    DataReader &DataReader::operator >> <uint16_t>(uint16_t &s)
	{
		_stupidlyReadIntegral<uint16_t>(s);

		return *this;
	}

    template <>
	DataReader &DataReader::operator >> <uint8_t>(uint8_t &b)
	{
		b = _getNext();

		return *this;
	}

	template <>
	DataReader &DataReader::operator >> <int64_t>(int64_t &l)
	{
		_stupidlyReadIntegral<int64_t>(l);

		return *this;
	}

    template <>
	DataReader &DataReader::operator >> <int32_t>(int32_t &i)
	{
		_stupidlyReadIntegral<int32_t>(i);

		return *this;
	}

    template <>
    DataReader &DataReader::operator >> <int16_t>(int16_t &s)
	{
		_stupidlyReadIntegral<int16_t>(s);

		return *this;
	}

    template <>
	DataReader &DataReader::operator >> <int8_t>(int8_t &b)
	{
		b = _getNext();

		return *this;
	}

    template <>
	DataReader &DataReader::operator >> <char>(char &b)
	{
		b = _getNext();

		return *this;
	}

    template <>
	DataReader &DataReader::operator >> <float>(float &f)
	{
		union
		{
			float v;
			char data[sizeof(v)];  // evil!
		};

		for(uint8_t i = 0; i < sizeof(v); ++i)
		{
			data[i] = _getNext();
		}

		f = v;

		return *this;
	}

    template <>
	DataReader &DataReader::operator >> <double>(double &d)
	{
		union
		{
			double v;
			char data[sizeof(v)];  // evil!
		};

		for(uint8_t i = 0; i < sizeof(v); ++i)
		{
			data[i] = _getNext();
		}

		d = v;

		return *this;
	}

    template <>
	DataReader &DataReader::operator >> <std::string>(std::string &s)
	{
		uint16_t len;
		*this >> len;
		if(len == 0)
		{
			s = "";
			return *this;
		}

		std::vector<char> buf(len+1, 0);
		read(buf.data(), len);

		s.assign(buf.data());

		return *this;
	}


    DataWriter::DataWriter(std::ostream &out)
    : mStream(&out)
    {
    }

    // you figure out this SFINAE stuff. I'm done.

    template <typename T>
    static void _writeIntegral(std::ostream &out, T value)
    {
        static_assert(std::is_integral<T>::value, "T must be integral");

        auto uvalue = static_cast<typename std::make_unsigned<T>::type>(value);

        // turn value into byte array first, then write it out (only one IO call).
        //  using bitshifts makes this endianess-independent.
        char bytes[sizeof(T)];
        for(size_t i = 0; i < sizeof(T); ++i)
        {
            bytes[i] = (uvalue >> i*8) & 0xff;
        }

        out.write(&bytes[0], sizeof(T));
    }

    template <>
    void DataWriter::writeTyped<uint8_t>(const uint8_t &v)
    {
        _writeIntegral(*mStream, v);
    }

    template <>
    void DataWriter::writeTyped<uint16_t>(const uint16_t &v)
    {
        _writeIntegral(*mStream, v);
    }

    template <>
    void DataWriter::writeTyped<uint32_t>(const uint32_t &v)
    {
        _writeIntegral(*mStream, v);
    }

    template <>
    void DataWriter::writeTyped<uint64_t>(const uint64_t &v)
    {
        _writeIntegral(*mStream, v);
    }

    template <>
    void DataWriter::writeTyped<float>(const float &v)
    {
        auto valuePtr = reinterpret_cast<const char*>(&v); // FIXME: host-endianess-dependent
        mStream->write(valuePtr, sizeof(v));
    }

    template <>
    void DataWriter::writeTyped<double>(const double &v)
    {
        auto valuePtr = reinterpret_cast<const char*>(&v); // FIXME: host-endianess-dependent
        mStream->write(valuePtr, sizeof(v));
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

}
