/*
 * SoundFactory.cpp
 *
 *  Created on: 23 Apr 2018
 *      Author: zal
 */

#include <odCore/db/SoundFactory.h>

#include <odCore/db/Database.h>

namespace od
{

    SoundFactory::SoundFactory(AssetProvider &ap, SrscFile &soundContainer)
    : AssetFactory<Sound>(ap, soundContainer)
    {
    }

    osg::ref_ptr<Sound> SoundFactory::loadAsset(RecordId soundId)
    {
        SrscFile::DirIterator dirIt = getSrscFile().getDirIteratorByTypeId(SrscRecordType::SOUND, soundId);
        if(dirIt == getSrscFile().getDirectoryEnd())
        {
            return nullptr;
        }

        osg::ref_ptr<Sound> sound(new Sound(getAssetProvider(), soundId));
        DataReader dr(getSrscFile().getStreamForRecord(dirIt));
        sound->loadFromRecord(dr);

        return sound;
    }

}



