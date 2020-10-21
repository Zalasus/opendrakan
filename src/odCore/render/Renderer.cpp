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
#include <odCore/db/Texture.h>

#include <odCore/render/Handle.h>

namespace odRender
{

    std::shared_ptr<Model> Renderer::getOrCreateModelFromDb(std::shared_ptr<odDb::Model> model)
    {
        OD_CHECK_ARG_NONNULL(model);

        if(!model->getCachedRenderModel().expired())
        {
            return std::shared_ptr<odRender::Model>(model->getCachedRenderModel());
        }

        auto newRenderModel = this->createModelFromDb(model);
        model->getCachedRenderModel() = newRenderModel;
        return newRenderModel;
    }

    std::shared_ptr<Image> Renderer::getOrCreateImageFromDb(std::shared_ptr<odDb::Texture> dbTexture)
    {
        OD_CHECK_ARG_NONNULL(dbTexture);

        if(!dbTexture->getCachedRenderImage().expired())
        {
            return std::shared_ptr<odRender::Image>(dbTexture->getCachedRenderImage());
        }

        auto newRenderTexture = this->createImageFromDb(dbTexture);
        dbTexture->getCachedRenderImage() = newRenderTexture;
        return newRenderTexture;
    }

    std::shared_ptr<Handle> Renderer::createHandleFromObject(od::LevelObject &obj)
    {
        auto handle = this->createHandle(RenderSpace::LEVEL);

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
