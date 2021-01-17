
#include <odCore/physics/PhysicsSystem.h>

#include <odCore/Panic.h>
#include <odCore/LightCallback.h>

#include <odCore/db/Model.h>

namespace odPhysics
{

    std::shared_ptr<ModelShape> PhysicsSystem::getOrCreateModelShape(std::shared_ptr<odDb::Model> model)
    {
        OD_CHECK_ARG_NONNULL(model);

        if(!model->getCachedPhysicsShape().expired())
        {
            return std::shared_ptr<odPhysics::ModelShape>(model->getCachedPhysicsShape());
        }

        auto newShape = this->createModelShape(model);

        model->getCachedPhysicsShape() = newShape;

        return newShape;
    }

    void PhysicsSystem::dispatchLighting(std::shared_ptr<Handle> handle)
    {
        OD_CHECK_ARG_NONNULL(handle);

        LightHandle *lightHandle = handle->asLightHandle();
        if(lightHandle != nullptr)
        {
            lightHandle->clearLightAffection();

            PhysicsTypeMasks::Mask mask = PhysicsTypeMasks::LevelObject | PhysicsTypeMasks::Layer;
            ContactTestResultVector results;
            this->contactTest(handle, mask, results);

            for(auto &result : results)
            {
                od::LightCallback *callback = result.handle->getLightCallback();
                if(callback != nullptr)
                {
                    callback->addAffectingLight(lightHandle->getLight());
                    lightHandle->addAffectedHandle(result.handle);
                }
            }

        }else
        {
            od::LightCallback *lightCallback = handle->getLightCallback();
            if(lightCallback == nullptr)
            {
                // since this handle has no light callback, dispatching lights to it will not do anything. thus, we can safely ignore the request
                return;
            }

            lightCallback->clearLightList();

            PhysicsTypeMasks::Mask mask = PhysicsTypeMasks::Light;
            ContactTestResultVector results;
            this->contactTest(handle, mask, results);

            for(auto &result : results)
            {
                auto lightHandle = result.handle->asLightHandle();
                lightCallback->addAffectingLight(lightHandle->getLight());
            }
        }
    }

}
