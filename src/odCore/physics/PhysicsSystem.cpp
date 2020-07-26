
#include <odCore/physics/PhysicsSystem.h>

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

}
