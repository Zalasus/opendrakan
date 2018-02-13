/*
 * DataStream.h
 *
 *  Created on: 30 Dec 2017
 *      Author: zal
 */

#ifndef INCLUDE_DATASTREAM_H_
#define INCLUDE_DATASTREAM_H_

#include <istream>

#include "Logger.h"

namespace od
{
	class DataReader
	{
	public:

	    class Ignore
	    {
	    public:
	        Ignore(size_t n);
	        inline size_t getCount() const { return mCountByte; }
	    private:
	        size_t mCountByte;
	    };

	    template <typename T>
	    class Expect
		{
		public:

	    	Expect(T value) : mValue(value) {}
	    	const T getExpectedValue() const { return mValue; }

		private:
	    	T mValue;
		};

		DataReader(std::istream &stream);

		template <typename T>
		DataReader &operator>>(T &s);

		DataReader &operator>>(const Ignore &s);

		template <typename T>
		DataReader &operator>>(const Expect<T> &s);

		void read(char *data, size_t size);

		void ignore(size_t n);
		void seek(size_t offset);
		size_t tell();

		std::istream &getStream();


	private:

		template <typename T>
        void _stupidlyReadIntegral(T &v)
        {
            union{ T vt; uint8_t vb[sizeof(T)];} vu;

            for(uint8_t i = 0; i < sizeof(T); ++i)
            {
                //TODO: find a better way to convert to little endian. this might be platform dependent

                vu.vb[i] = _getNext();
            }

            v = vu.vt;
        }

		uint8_t _getNext();

		std::istream &mStream;
	};

	template <typename T>
	DataReader &DataReader::operator>>(const DataReader::Expect<T> &s)
	{
		T value;
		*this >> value;

		if(value != s.getExpectedValue())
		{
			Logger::warn() << "Data error: expected value " << s.getExpectedValue() << ", found value " << value;
		}

		return *this;
	}

}

#endif /* INCLUDE_DATASTREAM_H_ */
