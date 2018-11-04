/*
 * AnimationFactory.h
 *
 *  Created on: 30 Mar 2018
 *      Author: zal
 */

#ifndef INCLUDE_DB_ANIMATIONFACTORY_H_
#define INCLUDE_DB_ANIMATIONFACTORY_H_

#include <odCore/db/Animation.h>
#include <odCore/db/AssetFactory.h>

namespace odDb
{

    class AnimationFactory : public AssetFactory<Animation>
    {
    public:

        AnimationFactory(AssetProvider &ap, od::SrscFile &animationContainer);


    protected:

        virtual AssetPtr<Animation> loadAsset(od::RecordId animId) override;

    };

}

#endif /* INCLUDE_DB_ANIMATIONFACTORY_H_ */
