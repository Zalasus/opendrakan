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

    od::RefPtr<Handle> Renderer::createHandleFromObject(od::LevelObject &obj)
    {
        auto handle = createHandle(RenderSpace::LEVEL);

        std::lock_guard<std::mutex> lock(handle->getMutex());

        handle->setPosition(obj.getPosition());
        handle->setOrientation(obj.getRotation());
        handle->setScale(obj.getScale());

        if(obj.getClass()->hasModel())
        {
            auto model = obj.getClass()->getModel()->getOrCreateRenderModel(this);
            handle->setModel(model);
        }

        return handle;
    }

}

