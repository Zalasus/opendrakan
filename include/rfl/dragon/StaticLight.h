/*
 * StaticLight.h
 *
 *  Created on: Aug 15, 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_STATICLIGHT_H_
#define INCLUDE_RFL_DRAGON_STATICLIGHT_H_

#include "rfl/RflClass.h"
#include "rfl/RflField.h"

#include "light/Light.h"

namespace odRfl
{

    class StaticLight : public RflClass
    {
    public:

        StaticLight();

        virtual void probeFields(RflFieldProbe &probe) override;
        virtual void spawned(od::LevelObject &obj) override;


    protected:

        RflColor        mColor;
        RflFloat        mIntensityScaling;
        RflFloat        mRadius;
        RflTextureRef   mLightMap;
        RflEnum         mQualityLevelRequired;

        osg::ref_ptr<od::Light> mLightHandle;
    };

}


#endif /* INCLUDE_RFL_DRAGON_STATICLIGHT_H_ */
