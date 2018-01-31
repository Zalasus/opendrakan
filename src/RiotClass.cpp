/*
 * RiotClass.cpp
 *
 *  Created on: 13 Jan 2018
 *      Author: zal
 */

#include "RiotClass.h"

namespace od
{

    RiotClass::RiotClass()
    : mClassTypeId(0)
    {
    }

    void RiotClass::loadFromRecord(SrscFile &srscFile, RecordId id)
    {
        DataReader dr(srscFile.getStreamForRecordTypeId(0x0102, id));

        dr >> mClassName;
        dr.ignore(2);

        dr.ignore(4); // model stuff

        dr >> mClassTypeId;



    }

    std::string RiotClass::getName()
    {
        return mClassName;
    }

    OD_REGISTER_ASSET_TYPE(ASSET_CLASS, RiotClass, "odb");

}



