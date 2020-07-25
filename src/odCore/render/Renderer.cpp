/*
 * Renderer.cpp
 *
 *  Created on: May 25, 2019
 *      Author: zal
 */

#include <odCore/render/Renderer.h>

#include <odCore/LevelObject.h>

#include <odCore/db/Class.h>
#include <odCore/db/Model.h>

#include <odCore/render/Handle.h>

namespace odRender
{

    std::shared_ptr<Handle> Renderer::createHandleFromObject(od::LevelObject &obj)
    {
        auto handle = this->createHandle(RenderSpace::LEVEL);

        std::lock_guard<std::mutex> lock(handle->getMutex());

        handle->setPosition(obj.getPosition());
        handle->setOrientation(obj.getRotation());
        handle->setScale(obj.getScale());

        if(obj.getClass()->hasModel())
        {
            auto model = this->createModelFromDb(obj.getClass()->getModel());
            handle->setModel(model);
        }

        return handle;
    }

}
