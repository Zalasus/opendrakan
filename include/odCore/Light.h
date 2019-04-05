/*
 * Light.h
 *
 *  Created on: 27 Nov 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_LIGHT_H_
#define INCLUDE_ODCORE_RENDER_LIGHT_H_

#include <vector>

#include <glm/vec3.hpp>

#include <odCore/RefCounted.h>
#include <odCore/BoundingSphere.h>
#include <odCore/LightCallback.h>

namespace odPhysics
{
    class PhysicsSystem;
    class Handle;
    class LightHandle;
}

namespace od
{
    class LevelObject;
    class Layer;

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
        inline bool isDynamic() const { return mIsDynamic; }
        inline void setColor(const glm::vec3 &color) { mColor = color; }
        inline void setIntensityScaling(float f) { mIntensityScaling = f; }
        inline void setRequiredQualityLevel(uint32_t ql) { mRequiredQualityLevel = ql; }
        inline void setDynamic(bool b) { mIsDynamic = b; }
        inline const std::vector<Layer*> &getAffectedLayers() const { return mAffectedLayers; }

        void setPosition(const glm::vec3 &p);
        void setRadius(float f);

        bool affects(const glm::vec3 &point);
        bool affects(const od::BoundingSphere &sphere);
        float distanceToPoint(const glm::vec3 &point);

        void addAffected(odPhysics::Handle *handle);

        void updateAffectedList();


    private:

        odPhysics::PhysicsSystem &mPhysicsSystem;
        od::RefPtr<odPhysics::LightHandle> mLightHandle;

        float mRadius;
        glm::vec3 mPosition;
        glm::vec3 mColor;
        float mIntensityScaling;
        uint32_t mRequiredQualityLevel;
        bool mIsDynamic;

        std::vector<Layer*> mAffectedLayers;
        std::vector<LightCallback*> mAffectedCallbacks;
    };

}

#endif /* INCLUDE_ODCORE_RENDER_LIGHT_H_ */
