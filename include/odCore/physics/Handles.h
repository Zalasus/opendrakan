/*
 * Handles.h
 *
 *  Created on: 8 Feb 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_PHYSICS_HANDLES_H_
#define INCLUDE_ODCORE_PHYSICS_HANDLES_H_

#include <memory>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

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

    /*
     * Note: This inherits from std::enable_shared_from_this because the Bullet implementation gives us no option to
     * directly store a weak_ptr in a btCollisionObject, but callbacks need to get owned references to the associated handle.
     * Unfortunately, that shortcoming forces us to make all handles inherit this. This will hopefully change in the future.
     */
    class Handle : public std::enable_shared_from_this<Handle>
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

        /**
         * @brief Sets the radius around the center in which objects are affected by this.
         *
         * If modifyLight is true (the default), the underlying light source will be modified to reflect
         * this change (a light source's rendered radius and it's radius of effect may differ).
         *
         * Note that this will not trigger a light dispatch at the moment. You have to call PhysicsSystem::dispatchLighting()
         * yourself to make other objects notice this.
         *
         * @param modifyLight  If true, the underlying light source will also adopt this value.
         */
        virtual void setRadius(float radius, bool modifyLight = true) = 0;

        /**
         * @brief Sets the position/center of this light.
         *
         * If modifyLight is true (the default), the underlying light source will be modified to reflect
         * this change (a light source's rendered position and it's center of effect may differ).
         *
         * Note that this will not trigger a light dispatch at the moment. You have to call PhysicsSystem::dispatchLighting()
         * yourself to make other objects notice this.
         *
         * @param modifyLight  If true, the underlying light source will also adopt this value.
         */
        virtual void setPosition(const glm::vec3 &pos, bool modifyLight = true) = 0;

        /**
         * @brief Returns the internal light source that determines how this LightHandle will be rendered.
         *
         * Changes of the returned reference's properties will be reflected by the renderer. Note that
         * changing the returned object's position or radius will *not* cause light affection to be updated.
         */
        virtual std::shared_ptr<od::Light> getLight() = 0;

        /**
         * @brief Registers the passed handle as being affected by this light.
         */
        void addAffectedHandle(std::shared_ptr<Handle> handle);

        /**
         * @brief Removes this light from all affected handles, then clears the list of affected handles.
         *
         * This is used before updating a light so all objects that were previously affected are cleared.
         * Should they turn out to still be affected, they will be re-added by the update.
         */
        void clearLightAffection();


    private:

        std::vector<std::weak_ptr<Handle>> mAffectedHandles;

    };

}

#endif /* INCLUDE_ODCORE_PHYSICS_HANDLES_H_ */
