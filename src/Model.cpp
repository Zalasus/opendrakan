/*
 * Model.cpp
 *
 *  Created on: 2 Feb 2018
 *      Author: zal
 */

#include "Model.h"

namespace od
{

	Model::Model()
	: mModelName("")
	{

	}

	void Model::loadFromRecord(SrscFile &srscFile, RecordId id)
	{
		//_loadNameAndShading(DataReader(srscFile.getStreamForRecordTypeId(0x0200, id)));
	}

	void Model::_loadNameAndShading(DataReader dr)
	{
		dr >> mModelName;

		uint32_t shadingType;
		dr >> shadingType;
	}

}

