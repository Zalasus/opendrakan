/*
 * MGFDataReader.cpp
 *
 *  Created on: 05.07.2014
 *      Author: Zalasus
 */

#include "DataStream.h"

#include <string>

#include "Exception.h"

namespace od
{

    DataReader::Ignore::Ignore(size_t n)
    : mCountByte(n)
    {
    }

	DataReader::DataReader(std::istream &stream)
	: mStream(stream)
	{
		if(!mStream.good())
		{
			throw IoException("Constructed DataReader with bad stream");
		}
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
		mStream.seekg(offset);
	}

	size_t DataReader::tell()
	{
		return mStream.tellg();
	}

	void DataReader::read(char *data, size_t size)
	{
		for(uint32_t i = 0; i < size; ++i)
		{
			data[i] = _getNext();
		}
	}

	std::istream &DataReader::getStream()
	{
	    return mStream;
	}

	uint8_t DataReader::_getNext()
	{
		int c = mStream.get();

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

    //FIXME: Portability issue. This could produce invalid results if target platform uses different floating point format than the advised IEEE 754
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

	//FIXME: Portability issue. This could produce invalid results if target platform uses different floating point format than the advised IEEE 754
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

    // is this really a primitive?
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

		char *buf = new char[len+1];
		buf[len] = 0; // some strings seem to be terminated, some are not. add terminator just to be sure
		read(buf, len);

		s = std::string(buf);

		delete[] buf;

		return *this;
	}


    MemBuffer::MemBuffer(char *begin, char *end)
    : mBegin(begin)
    , mEnd(end)
    {
    	this->setg(begin, begin, end);
    }

    std::streampos MemBuffer::seekoff(std::streamoff off, std::ios_base::seekdir way, std::ios_base::openmode which)
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

    std::streampos MemBuffer::seekpos(std::streampos sp, std::ios_base::openmode which)
    {
    	return seekoff(sp - pos_type(off_type(0)), std::ios_base::beg, which);
    }

}
