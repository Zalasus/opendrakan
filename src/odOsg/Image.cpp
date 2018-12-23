/*
 * Image.cpp
 *
 *  Created on: Dec 12, 2018
 *      Author: zal
 */

#include <odOsg/Image.h>

#include <odCore/Exception.h>

#include <odCore/db/Texture.h>

#include <odOsg/Texture.h>

namespace odOsg
{

    Image::Image(odDb::Texture *dbTexture)
    : mDbTexture(dbTexture)
    {
        mOsgImage = new osg::Image;
        mOsgImage->setImage(mDbTexture->getWidth(), mDbTexture->getHeight(), 1, 4, GL_RGBA, GL_UNSIGNED_BYTE,
                mDbTexture->getRawR8G8B8A8Data(), osg::Image::NO_DELETE);
    }

    Image::~Image()
    {
    }

    glm::vec2 Image::getDimensionsUV()
    {
        return glm::vec2(mDbTexture->getWidth(), mDbTexture->getHeight());
    }

    od::RefPtr<odRender::Texture> Image::createTexture()
    {
        return od::make_refd<Texture>(this);
    }

    od::RefPtr<odRender::Texture> Image::getTextureForUsage(odRender::TextureUsage usage)
    {
        switch(usage)
        {
        case odRender::TextureUsage::Model:
            if(mModelRenderTexture == nullptr)
            {
                od::RefPtr<odRender::Texture> texture = createTexture();
                texture->setEnableWrapping(true);
                mModelRenderTexture = texture.release();
            }
            return mModelRenderTexture.get();

        case odRender::TextureUsage::Layer:
            if(mLayerRenderTexture == nullptr)
            {
                od::RefPtr<odRender::Texture> texture = createTexture();
                texture->setEnableWrapping(false);
                mLayerRenderTexture = texture.release();
            }
            return mLayerRenderTexture.get();

        default:
            return createTexture();
        }
    }

}


