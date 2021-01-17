/*
 * Image.cpp
 *
 *  Created on: Dec 12, 2018
 *      Author: zal
 */

#include <odOsg/render/Image.h>

#include <osg/ImageSequence>

#include <odCore/Panic.h>

#include <odCore/db/Texture.h>

#include <odOsg/render/Texture.h>

namespace odOsg
{

    Image::Image(std::shared_ptr<odDb::Texture> dbTexture)
    : mDbTexture(dbTexture)
    {
        if(dbTexture == nullptr)
        {
            OD_PANIC() << "Tried to create osg image from null texture";
        }

        mOsgImage = new osg::Image;
        mOsgImage->setImage(  dbTexture->getWidth(), dbTexture->getHeight(), 1, 4, GL_RGBA, GL_UNSIGNED_BYTE
                            , dbTexture->getRawR8G8B8A8Data(), osg::Image::NO_DELETE);

        if(dbTexture->isAnimation())
        {
            osg::ref_ptr<osg::ImageSequence> imgSequence = new osg::ImageSequence;
            imgSequence->setImage(0, mOsgImage);

            auto &nextFrames = dbTexture->getNextAnimationFrames();
            for(size_t i = 0; i < nextFrames.size(); ++i)
            {
                auto frame = nextFrames[i];
                osg::ref_ptr<osg::Image> img = new osg::Image;
                img->setImage(  frame->getWidth(), frame->getHeight(), 1, 4, GL_RGBA, GL_UNSIGNED_BYTE
                              , frame->getRawR8G8B8A8Data(), osg::Image::NO_DELETE);

                imgSequence->setImage(i+1, img);
            }

            imgSequence->setLength(((double)dbTexture->getAnimationFrameCount())/dbTexture->getAnimationFPS());
            imgSequence->play();

            mOsgImage = imgSequence;
        }
    }

    Image::~Image()
    {
    }

    glm::vec2 Image::getDimensionsUV()
    {
        return glm::vec2(mDbTexture->getWidth(), mDbTexture->getHeight());
    }

    /*
    od::RefPtr<odRender::Texture> Image::getTextureForUsage(odRender::TextureUsage usage)
    {
        switch(usage)
        {
        case odRender::TextureUsage::Model:
            if(mModelRenderTexture.isNull())
            {
                od::RefPtr<odRender::Texture> texture = createTexture();
                texture->setEnableWrapping(true);
                mModelRenderTexture = texture.get();
                return texture;
            }
            return mModelRenderTexture.aquire();

        case odRender::TextureUsage::Layer:
            if(mLayerRenderTexture.isNull())
            {
                od::RefPtr<odRender::Texture> texture = createTexture();
                texture->setEnableWrapping(false);
                mLayerRenderTexture = texture.get();
                return texture;
            }
            return mLayerRenderTexture.aquire();

        default:
            return createTexture();
        }
    }*/

}
