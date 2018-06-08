/*
 * SequenceFactory.cpp
 *
 *  Created on: 2 Mar 2018
 *      Author: zal
 */

#include "db/SequenceFactory.h"

#include "db/Database.h"

namespace od
{

    SequenceFactory::SequenceFactory(const FilePath &sdbFilePath, Database &database)
    : AssetFactory<Sequence>(sdbFilePath, database)
    {
    }

    SequencePtr SequenceFactory::loadAsset(RecordId assetId)
    {
        SrscFile::DirIterator dirIt = getSrscFile().getDirIteratorByTypeId(SrscRecordType::SEQUENCE, assetId);
        if(dirIt == getSrscFile().getDirectoryEnd())
        {
            Logger::error() << "Sequence " << std::hex << assetId << std::dec << " not found in database " << getDatabase().getShortName();
            throw NotFoundException("Sequence not found in database");
        }

        Logger::debug() << "Loading sequence " << std::hex << assetId << std::dec << " from database '" << getDatabase().getDbFilePath().fileStrNoExt() << "'";

        SequencePtr sequence(new Sequence(getDatabase(), assetId));
        DataReader dr(getSrscFile().getStreamForRecord(dirIt));
        sequence->loadFromRecord(dr);

        return sequence;
    }

}

