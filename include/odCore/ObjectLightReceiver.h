/*
 * ObjectLightReceiver.h
 *
 *  Created on: Apr 5, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_OBJECTLIGHTRECEIVER_H_
#define INCLUDE_ODCORE_RENDER_OBJECTLIGHTRECEIVER_H_

#include <vector>

#include <odCore/LightCallback.h>

namespace odPhysics
{
    class PhysicsSystem;
    class ObjectHandle;
}

namespace odRender
{
    class Handle;
}

namespace od
{

    class ObjectNode;

    /**
     * Implementation of a LightCallback that redirects received lights to a render node and
     * also provides a method to query all lights affecting an object physics handle.
     */
    class ObjectLightReceiver : public od::LightCallback
    {
    public:

        /**
         * @param physicsHandle  Physics object handle to use for light queries. Not nullptr
         * @param renderHandle   Render handle to receive the discovered lights. May be nullptr
         */
        ObjectLightReceiver(odPhysics::PhysicsSystem &ps, std::shared_ptr<odPhysics::ObjectHandle> physicsHandle, std::shared_ptr<odRender::Handle> renderHandle);
        virtual ~ObjectLightReceiver();

        virtual void removeAffectingLight(od::Light *light) override;
        virtual void addAffectingLight(od::Light *light) override;
        virtual void clearLightList() override;

        void updateAffectingLights();


    private:

        odPhysics::PhysicsSystem &mPhysicsSystem;
        std::shared_ptr<odPhysics::ObjectHandle> mPhysicsHandle;
        std::shared_ptr<odRender::Handle> mRenderHandle;

        std::vector<std::shared_ptr<Light>> mAffectingLights;

    };

}


#endif /* INCLUDE_ODCORE_RENDER_OBJECTLIGHTRECEIVER_H_ */
