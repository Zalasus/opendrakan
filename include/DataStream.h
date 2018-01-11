/*
 * DataStream.h
 *
 *  Created on: 30 Dec 2017
 *      Author: zal
 */

#ifndef INCLUDE_DATASTREAM_H_
#define INCLUDE_DATASTREAM_H_

#include <istream>

namespace od
{
	class DataReader
	{
	public:

		DataReader(std::istream &stream);

		template <typename T>
		DataReader &operator>>(T &s);

		void read(char *data, size_t size);

		void ignore(size_t n);


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

}

#endif /* INCLUDE_DATASTREAM_H_ */
