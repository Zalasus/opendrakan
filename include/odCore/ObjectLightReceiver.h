/*
 * ObjectLightReceiver.h
 *
 *  Created on: Apr 5, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_OBJECTLIGHTRECEIVER_H_
#define INCLUDE_ODCORE_RENDER_OBJECTLIGHTRECEIVER_H_

#include <odCore/LightCallback.h>
#include <odCore/RefCounted.h>

namespace odPhysics
{
    class PhysicsSystem;
    class ObjectHandle;
}

namespace odRender
{
    class ObjectNode;
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

        ObjectLightReceiver(odPhysics::PhysicsSystem &ps, odPhysics::ObjectHandle *oh, odRender::ObjectNode *on);
        virtual ~ObjectLightReceiver();

        virtual void removeAffectingLight(od::Light *light) override;
        virtual void addAffectingLight(od::Light *light) override;
        virtual void clearLightList() override;

        void updateAffectingLights();


    private:

        odPhysics::PhysicsSystem &mPhysicsSystem;
        od::RefPtr<odPhysics::ObjectHandle> mPhysicsHandle;
        od::RefPtr<odRender::ObjectNode> mRenderNode;

    };

}


#endif /* INCLUDE_ODCORE_RENDER_OBJECTLIGHTRECEIVER_H_ */
