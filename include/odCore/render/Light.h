/*
 * Light.h
 *
 *  Created on: 27 Nov 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_LIGHT_H_
#define INCLUDE_ODCORE_RENDER_LIGHT_H_

#include <glm/vec3.hpp>

#include <odCore/RefCounted.h>
#include <odCore/BoundingSphere.h>

namespace od
{
    class LevelObject;
}

namespace odRender
{

    class Light : public od::RefCounted
    {
    public:

        struct DefaultLightGroups
        {
            static const uint32_t All = -1;
            static const uint32_t LevelObjects = 1;
            static const uint32_t Gui = 2;
        };

        Light(od::LevelObject *obj);

        inline od::LevelObject *getLevelObject() { return mLevelObject; }
        inline glm::vec3 getColor() const { return mColor; }
        inline float getIntensityScaling() const { return mIntensityScaling; }
        inline float getRadius() const { return mRadius; }
        inline uint32_t getRequiredQualityLevel() const { return mRequiredQualityLevel; }
        inline void setColor(const glm::vec3 &color) { mColor = color; }
        inline void setIntensityScaling(float f) { mIntensityScaling = f; }
        inline void setRadius(float f) { mRadius = f; }
        inline void setRequiredQualityLevel(uint32_t ql) { mRequiredQualityLevel = ql; }
        inline uint32_t getLightGroup() const { return mLightGroup; }
        inline void setLightGroup(uint32_t lightGroup) { mLightGroup = lightGroup; }

        bool affects(const glm::vec3 &point);
        bool affects(const od::BoundingSphere &sphere);
        float distanceToPoint(const glm::vec3 &point);


    private:

        od::LevelObject *mLevelObject;
        glm::vec3 mColor;
        float mIntensityScaling;
        float mRadius;
        uint32_t mRequiredQualityLevel;

        uint32_t mLightGroup;
    };

}

#endif /* INCLUDE_ODCORE_RENDER_LIGHT_H_ */
