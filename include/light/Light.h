/*
 * Light.h
 *
 *  Created on: Aug 15, 2018
 *      Author: zal
 */

#ifndef INCLUDE_LIGHT_LIGHT_H_
#define INCLUDE_LIGHT_LIGHT_H_

#include <osg/Light>

namespace od
{

    class LevelObject;

    class LightHandle : public osg::Referenced
    {
    public:

        LightHandle(LevelObject *obj, float radius, uint32_t requiredQualityLevel);

        inline LevelObject *getLevelObject() { return mLevelObject; }
        inline float getRadius() const { return mRadius; }
        inline uint32_t getRequiredQualityLevel() const { return mRequiredQualityLevel; }
        inline osg::Light *getLight() { return mLight; }


    private:

        osg::ref_ptr<LevelObject> mLevelObject;
        float mRadius;
        uint32_t mRequiredQualityLevel;
        osg::ref_ptr<osg::Light> mLight;

    };

}


#endif /* INCLUDE_LIGHT_LIGHT_H_ */
