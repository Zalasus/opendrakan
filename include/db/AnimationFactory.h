/*
 * AnimationFactory.h
 *
 *  Created on: 30 Mar 2018
 *      Author: zal
 */

#ifndef INCLUDE_DB_ANIMATIONFACTORY_H_
#define INCLUDE_DB_ANIMATIONFACTORY_H_

#include "Animation.h"
#include "AssetFactory.h"

namespace od
{

    class AnimationFactory : public AssetFactory<Animation>
    {
    public:

        AnimationFactory(AssetProvider &ap, SrscFile &animationContainer);


    protected:

        virtual osg::ref_ptr<Animation> loadAsset(RecordId animId) override;

    };

}

#endif /* INCLUDE_DB_ANIMATIONFACTORY_H_ */
