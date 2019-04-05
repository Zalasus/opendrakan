/*
 * DefaultObjectClass.h
 *
 *  Created on: Apr 2, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RFL_DEFAULTOBJECTCLASS_H_
#define INCLUDE_ODCORE_RFL_DEFAULTOBJECTCLASS_H_

#include <memory>

#include <odCore/RefCounted.h>
#include <odCore/ObjectLightReceiver.h>

#include <odCore/render/ObjectNode.h>

#include <odCore/physics/Handles.h>

#include <odCore/rfl/RflClass.h>

namespace odRender
{
    class ObjectNode;
}

namespace odPhysics
{
    class ObjectHandle;
}

namespace odRfl
{

    /**
     * A default class for level objects whose RFL class type could not be found in the RFL.
     *
     * This is a fallback that should only be used as long as OpenDrakan's RFL does not define all
     * RFL classes. As soon as that is the case, not being able to locate an RFL class should warrant
     * an exception or rejection of the class database.
     */
    class DefaultObjectClass : public RflClass
    {
    public:

        virtual ~DefaultObjectClass();

        virtual void probeFields(FieldProbe &probe) override;

        virtual void onSpawned(od::LevelObject &obj) override;
        virtual void onDespawned(od::LevelObject &obj) override;
        virtual void onMoved(od::LevelObject &obj) override;


    private:

        void _updateLighting(od::LevelObject &obj);

        od::RefPtr<odRender::ObjectNode> mRenderNode;
        od::RefPtr<odPhysics::ObjectHandle> mPhysicsHandle;

        std::unique_ptr<od::ObjectLightReceiver> mLightReceiver;

    };

}


#endif /* INCLUDE_ODCORE_RFL_DEFAULTOBJECTCLASS_H_ */
