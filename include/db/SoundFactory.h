/*
 * SoundFactory.h
 *
 *  Created on: 23 Apr 2018
 *      Author: zal
 */

#ifndef SOUNDFACTORY_H_
#define SOUNDFACTORY_H_

#include "db/AssetFactory.h"
#include "db/Sound.h"

namespace od
{
    class Database;

    class SoundFactory : public AssetFactory<Sound>
    {
    public:

        SoundFactory(const FilePath &sdbFilePath, Database &database);


    protected:

        // implement AssetFactory<Texture>
        osg::ref_ptr<Sound> loadAsset(RecordId soundId) override;
    };
}


#endif /* SOUNDFACTORY_H_ */
