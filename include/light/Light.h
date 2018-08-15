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

    class Light : public osg::Referenced
    {
    public:

        Light(LevelObject *obj);

        inline void setDiffuseColor(const osg::Vec4 &color) { mDiffuseColor = color; }
        inline void setIntensity(float intensity) { mIntensity = intensity; }
        inline void setRadius(float radius) { mRadius = radius; }


    private:

        osg::ref_ptr<LevelObject> mLevelObject;
        osg::Vec4 mDiffuseColor;
        float mIntensity;
        float mRadius;

    };

}


#endif /* INCLUDE_LIGHT_LIGHT_H_ */
