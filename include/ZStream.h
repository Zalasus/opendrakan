/*
 * ZStream.h
 *
 *  Created on: 30 Dec 2017
 *      Author: zal
 */

#ifndef INCLUDE_ZSTREAM_H_
#define INCLUDE_ZSTREAM_H_

#include <istream>

namespace od
{

	class ZStream : std::istream
	{
	public:

		ZStream(std::istream &in);



	private:

		std::istream &mInputStream;

	};

}

#endif /* INCLUDE_ZSTREAM_H_ */
