/*
 * SoundFactory.cpp
 *
 *  Created on: 23 Apr 2018
 *      Author: zal
 */

#include <odCore/db/SoundFactory.h>

#include <odCore/db/Database.h>

namespace odDb
{

    SoundFactory::SoundFactory(AssetProvider &ap, od::SrscFile &soundContainer)
    : AssetFactory<Sound>(ap, soundContainer)
    {
    }

    od::RefPtr<Sound> SoundFactory::loadAsset(od::RecordId soundId)
    {
        auto cursor = getSrscFile().getFirstRecordOfTypeId(od::SrscRecordType::SOUND, soundId);
        if(!cursor.isValid())
        {
            return nullptr;
        }

        od::RefPtr<Sound> sound = od::make_refd<Sound>(getAssetProvider(), soundId);
        od::DataReader dr(cursor.getReader());
        sound->loadFromRecord(dr);

        return sound;
    }

}



