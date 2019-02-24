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

namespace odPhysics
{
    class PhysicsSystem;
    class LightHandle;
}

namespace od
{
    class LevelObject;

    class Light : public od::RefCounted
    {
    public:

        Light(odPhysics::PhysicsSystem &physicsSystem);
        ~Light();

        inline glm::vec3 getColor() const { return mColor; }
        inline float getIntensityScaling() const { return mIntensityScaling; }
        inline float getRadius() const { return mRadius; }
        inline glm::vec3 getPosition() const { return mPosition; }
        inline uint32_t getRequiredQualityLevel() const { return mRequiredQualityLevel; }
        inline void setColor(const glm::vec3 &color) { mColor = color; }
        inline void setIntensityScaling(float f) { mIntensityScaling = f; }
        inline void setRequiredQualityLevel(uint32_t ql) { mRequiredQualityLevel = ql; }

        void setPosition(const glm::vec3 &p);
        void setRadius(float f);

        bool affects(const glm::vec3 &point);
        bool affects(const od::BoundingSphere &sphere);
        float distanceToPoint(const glm::vec3 &point);


    private:

        odPhysics::PhysicsSystem &mPhysicsSystem;
        od::RefPtr<odPhysics::LightHandle> mLightHandle;

        float mRadius;
        glm::vec3 mPosition;
        glm::vec3 mColor;
        float mIntensityScaling;
        uint32_t mRequiredQualityLevel;
    };

}

#endif /* INCLUDE_ODCORE_RENDER_LIGHT_H_ */
