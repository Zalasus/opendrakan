/*
 * DefaultObjectClass.h
 *
 *  Created on: Apr 2, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RFL_DEFAULTOBJECTCLASS_H_
#define INCLUDE_ODCORE_RFL_DEFAULTOBJECTCLASS_H_

#include <memory>

#include <odCore/ObjectLightReceiver.h>

#include <odCore/render/Handle.h>

#include <odCore/physics/Handles.h>

#include <odCore/rfl/Class.h>

namespace odRfl
{

    /**
     * A default class for level objects whose RFL class type could not be found in the RFL.
     *
     * This is a fallback that should only be used as long as OpenDrakan's RFL does not define all
     * RFL classes. As soon as that is the case, not being able to locate an RFL class should warrant
     * an exception or rejection of the class database.
     */
    class DefaultObjectClass : public LevelObjectClassBase
    {
    public:

        DefaultObjectClass();
        virtual ~DefaultObjectClass();

        virtual void probeFields(FieldProbe &probe) override;

        virtual void onSpawned() override;
        virtual void onDespawned() override;
        virtual void onVisibilityChanged() override;
        virtual void onLayerChanged(od::Layer *from, od::Layer *to) override;
        virtual void onTransformChanged() override;

        static std::unique_ptr<DefaultObjectClass> makeInstance();

    protected:

        std::shared_ptr<odRender::Handle> mRenderHandle;
        std::shared_ptr<odPhysics::ObjectHandle> mPhysicsHandle;

        std::unique_ptr<od::ObjectLightReceiver> mLightReceiver;

    };


    class DefaultServerObjectClass : public odRfl::LevelObjectClassBase
    {
    public:

        virtual void probeFields(FieldProbe &probe) override;


    private:

        std::shared_ptr<odPhysics::ObjectHandle> mPhysicsHandle;

    };


    class DefaultClientObjectClass : public odRfl::LevelObjectClassBase
    {
    public:

        virtual void probeFields(FieldProbe &probe) override;

        virtual void onSpawned() override;
        virtual void onDespawned() override;
        virtual void onVisibilityChanged() override;

        // assume default objects are static, so we don't need these:
        //virtual void onLayerChanged(od::Layer *from, od::Layer *to) override;
        //virtual void onTransformChanged() override;


    private:

        std::shared_ptr<odRender::Handle> mRenderHandle;
        std::shared_ptr<odPhysics::ObjectHandle> mPhysicsHandle;
        std::unique_ptr<od::ObjectLightReceiver> mLightReceiver;

    };

}


#endif /* INCLUDE_ODCORE_RFL_DEFAULTOBJECTCLASS_H_ */
