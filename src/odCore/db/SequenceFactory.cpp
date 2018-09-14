/*
 * SequenceFactory.cpp
 *
 *  Created on: 2 Mar 2018
 *      Author: zal
 */

#include <odCore/db/SequenceFactory.h>

#include <odCore/db/Database.h>

namespace od
{

    SequenceFactory::SequenceFactory(AssetProvider &ap, SrscFile &sequenceContainer)
    : AssetFactory<Sequence>(ap, sequenceContainer)
    {
    }

    osg::ref_ptr<Sequence> SequenceFactory::loadAsset(RecordId assetId)
    {
        SrscFile::DirIterator dirIt = getSrscFile().getDirIteratorByTypeId(SrscRecordType::SEQUENCE, assetId);
        if(dirIt == getSrscFile().getDirectoryEnd())
        {
            return nullptr;
        }

        osg::ref_ptr<Sequence> sequence(new Sequence(getAssetProvider(), assetId));
        DataReader dr(getSrscFile().getStreamForRecord(dirIt));
        sequence->loadFromRecord(dr);

        return sequence;
    }
}

