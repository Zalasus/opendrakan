/*
 * StaticLight.h
 *
 *  Created on: Aug 15, 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_STATICLIGHT_H_
#define INCLUDE_RFL_DRAGON_STATICLIGHT_H_

#include <odCore/rfl/RflClass.h>
#include <odCore/rfl/RflField.h>
#include <odCore/light/Light.h>

namespace od
{

    class StaticLight : public RflClass
    {
    public:

        StaticLight();

        virtual void probeFields(FieldProbe &probe) override;
        virtual void onLoaded(LevelObject &obj) override;
        virtual void onSpawned(LevelObject &obj) override;
        virtual void onMoved(LevelObject &obj) override;
        virtual void onDespawned(LevelObject &obj) override;


    protected:

        RflColor        mColor;
        RflFloat        mIntensityScaling;
        RflFloat        mRadius;
        RflTextureRef   mLightMap;
        RflEnum         mQualityLevelRequired;

        osg::ref_ptr<od::Light> mLight;
    };


    OD_DEFINE_RFLCLASS_TRAITS(DragonRfl, 0x0084, "Light Source", "Static Light", StaticLight);

}


#endif /* INCLUDE_RFL_DRAGON_STATICLIGHT_H_ */
