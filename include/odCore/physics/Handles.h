/*
 * Handles.h
 *
 *  Created on: 8 Feb 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_PHYSICS_HANDLES_H_
#define INCLUDE_ODCORE_PHYSICS_HANDLES_H_

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <odCore/RefCounted.h>

namespace od
{
    class LevelObject;
    class Layer;
    class Light;
    class LightCallback;
}

namespace odPhysics
{

    class ObjectHandle;
    class LayerHandle;
    class LightHandle;

    class Handle : public od::RefCounted
    {
    public:

        enum class Type
        {
            Layer,
            Object,
            Light
        };

        Handle();

        inline void setLightCallback(od::LightCallback *c) { mLightCallback = c; }
        inline od::LightCallback *getLightCallback() const { return mLightCallback; }

        /**
         * @brief Fast upcast. This avoids a dynamic_cast. Will return nullptr if not a layer handle.
         */
        virtual LayerHandle *asLayerHandle();

        /**
         * @brief Fast upcast. This avoids a dynamic_cast. Will return nullptr if not an object handle.
         */
        virtual ObjectHandle *asObjectHandle();

        /**
         * @brief Fast upcast. This avoids a dynamic_cast. Will return nullptr if not a light handle.
         */
        virtual LightHandle *asLightHandle();

        /**
         * @brief Returns the type of this handle.
         */
        virtual Type getHandleType() = 0;


    protected:

        od::LightCallback *mLightCallback;

    };


    class ObjectHandle : public Handle
    {
    public:

        virtual ObjectHandle *asObjectHandle() override;

        virtual Type getHandleType() override;

        /**
         * @brief Enables or disables collision detection. Detection is enabled by default.
         *
         * When disabled, all allocated resources will be kept. Only interaction with other collision objects
         * will be disabled. Thus, this method is good for short-term collision deactivation. If an object
         * should be removed from collision detection for longer (like when it despawns), it might be more
         * efficient to just drop the Handle and create a new one later.
         */
        virtual void setEnableCollision(bool b) = 0;

        virtual void setPosition(const glm::vec3 &p) = 0;
        virtual void setOrientation(const glm::quat &q) = 0;
        virtual void setScale(const glm::vec3 &s) = 0;

        virtual od::LevelObject &getLevelObject() = 0;
    };


    class LayerHandle : public Handle
    {
    public:

        virtual LayerHandle *asLayerHandle() override;

        virtual Type getHandleType() override;

        virtual od::Layer &getLayer() = 0;

    };


    class LightHandle : public Handle
    {
    public:

        virtual LightHandle *asLightHandle() override;

        virtual Type getHandleType() override;

        virtual void setRadius(float radius) = 0;
        virtual void setPosition(const glm::vec3 &pos) = 0;

        virtual od::Light &getLight() = 0;

    };

}

#endif /* INCLUDE_ODCORE_PHYSICS_HANDLES_H_ */
