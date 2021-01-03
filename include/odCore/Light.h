/*
 * Light.h
 *
 *  Created on: 27 Nov 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_LIGHT_H_
#define INCLUDE_ODCORE_RENDER_LIGHT_H_

#include <glm/vec3.hpp>

#include <odCore/BoundingSphere.h>

namespace od
{
    /**
     * @brief Simple structure representing the properties of a light source.
     *
     * This only encapsultes light source properties! The functionality of
     * dispatching light to other objects has been fully integrated into the
     * physics system.
     *
     * TODO: we might integrate some riot-specific light calculations into here
     * (like the ones used by Layer to bake lights into the vertex color)
     */
    class Light
    {
    public:

        Light();
        Light(const Light &l) = default;

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
        inline void setRadius(float r) { mRadius = r; }

        /**
         * @brief Sets the light's position.
         *
         * Note that this won't affect the set of influenced objects. That is handled by the physics system.
         * The type odPhysics::LightHandle has it's own setPosition method for that.
         */
        inline void setPosition(const glm::vec3 &p) { mPosition = p; }

        bool affects(const glm::vec3 &point);
        bool affects(const od::BoundingSphere &sphere);
        float distanceToPoint(const glm::vec3 &point);


    private:

        float mRadius;
        glm::vec3 mPosition;
        glm::vec3 mColor;
        float mIntensityScaling;
        uint32_t mRequiredQualityLevel;
        bool mIsDynamic;
    };

}

#endif /* INCLUDE_ODCORE_RENDER_LIGHT_H_ */
