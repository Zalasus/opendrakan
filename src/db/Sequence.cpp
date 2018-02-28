/*
 * Sequence.cpp
 *
 *  Created on: 28 Feb 2018
 *      Author: zal
 */

#include "db/Sequence.h"

namespace od
{

	void Sequence::loadSequenceRecord(DataReader dr)
	{
		uint32_t actorCount;

		dr >> mSequenceName
		   >> DataReader::Ignore(12)
		   >> actorCount;


	}

}
