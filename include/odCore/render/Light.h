/*
 * Light.h
 *
 *  Created on: Aug 15, 2018
 *      Author: zal
 */

#ifndef INCLUDE_LIGHT_LIGHT_H_
#define INCLUDE_LIGHT_LIGHT_H_

#include <stdint.h>
#include <stdlib.h>
#include <osg/Referenced>
#include <osg/ref_ptr>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/BoundingSphere>

namespace od
{
    class LevelObject;
}

namespace odRender
{

    class Light : public osg::Referenced
    {
    public:

        Light(od::LevelObject *obj);

        inline od::LevelObject *getLevelObject() { return mLevelObject; }
        inline osg::Vec3 getColor() const { return mColor; }
        inline float getIntensityScaling() const { return mIntensityScaling; }
        inline float getRadius() const { return mRadius; }
        inline uint32_t getRequiredQualityLevel() const { return mRequiredQualityLevel; }
        inline void setColor(const osg::Vec3 &color) { mColor = color; }
        inline void setIntensityScaling(float f) { mIntensityScaling = f; }
        inline void setRadius(float f) { mRadius = f; }
        inline void setRequiredQualityLevel(uint32_t ql) { mRequiredQualityLevel = ql; }

        bool affects(const osg::Vec3 &point);
        bool affects(const osg::BoundingSphere &sphere);
        float distanceToPoint(const osg::Vec3 &point);


    private:

        od::LevelObject *mLevelObject;
        osg::Vec3 mColor;
        float mIntensityScaling;
        float mRadius;
        uint32_t mRequiredQualityLevel;
    };

}


#endif /* INCLUDE_LIGHT_LIGHT_H_ */
