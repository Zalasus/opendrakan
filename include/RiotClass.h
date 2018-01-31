/*
 * RiotClass.h
 *
 *  Created on: 9 Jan 2018
 *      Author: zal
 */

#ifndef INCLUDE_RIOTCLASS_H_
#define INCLUDE_RIOTCLASS_H_

#include "RiotAsset.h"
#include "SrscFile.h"
#include "DataStream.h"
#include "rfl/RiotFunctionLibrary.h"

namespace od
{

	class RiotClass : public RiotAsset
	{
	public:

		RiotClass();

		virtual void loadFromRecord(SrscFile &srscFile, RecordId id);

		virtual std::string getName();


	private:

		std::string mClassName;
		RflClassId mClassTypeId;
	};

}

#endif /* INCLUDE_RIOTCLASS_H_ */
