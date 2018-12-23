/*
 * SequenceFactory.cpp
 *
 *  Created on: 2 Mar 2018
 *      Author: zal
 */

#include <odCore/db/SequenceFactory.h>

#include <odCore/db/Database.h>

namespace odDb
{

    SequenceFactory::SequenceFactory(AssetProvider &ap, od::SrscFile &sequenceContainer)
    : AssetFactory<Sequence>(ap, sequenceContainer)
    {
    }

    od::RefPtr<Sequence> SequenceFactory::loadAsset(od::RecordId assetId)
    {
        od::SrscFile::DirIterator dirIt = getSrscFile().getDirIteratorByTypeId(od::SrscRecordType::SEQUENCE, assetId);
        if(dirIt == getSrscFile().getDirectoryEnd())
        {
            return nullptr;
        }

        od::RefPtr<Sequence> sequence = od::make_refd<Sequence>(getAssetProvider(), assetId);
        od::DataReader dr(getSrscFile().getStreamForRecord(dirIt));
        sequence->loadFromRecord(dr);

        return sequence;
    }
}

