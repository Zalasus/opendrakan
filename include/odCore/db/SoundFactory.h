/*
 * SoundFactory.h
 *
 *  Created on: 23 Apr 2018
 *      Author: zal
 */

#ifndef SOUNDFACTORY_H_
#define SOUNDFACTORY_H_

#include <odCore/db/AssetFactory.h>
#include <odCore/db/Sound.h>

namespace odDb
{
    class Database;

    class SoundFactory : public AssetFactory<Sound>
    {
    public:

        SoundFactory(AssetProvider &ap, od::SrscFile &soundContainer);


    protected:

        // implement AssetFactory<Texture>
        virtual od::RefPtr<Sound> loadAsset(od::RecordId soundId) override;


    };
}


#endif /* SOUNDFACTORY_H_ */
