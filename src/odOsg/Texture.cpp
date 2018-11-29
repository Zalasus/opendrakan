/*
 * Texture.cpp
 *
 *  Created on: 23 Nov 2018
 *      Author: zal
 */

#include <odOsg/Texture.h>

#include <odCore/db/Texture.h>

namespace odOsg
{

    Texture::Texture(Renderer *renderer, odDb::Texture *dbTexture)
    : mDbTexture(dbTexture)
    {
        mImage = new osg::Image;
        mImage->setImage(mDbTexture->getWidth(), mDbTexture->getHeight(), 1, 4, GL_RGBA, GL_UNSIGNED_BYTE,
                mDbTexture->getRawR8G8B8A8Data(), osg::Image::NO_DELETE);
    }

    Texture::~Texture()
    {
    }

    osg::Texture2D *Texture::getOsgTexture(bool clamping)
    {
        if(clamping)
        {
            if(mClampingTexture == nullptr)
            {
                mClampingTexture = new osg::Texture2D(mImage);
                mClampingTexture->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);
                mClampingTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
                mClampingTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
            }

            return mClampingTexture;

        }else
        {
            if(mWrappingTexture == nullptr)
            {
                mWrappingTexture = new osg::Texture2D(mImage);
                mWrappingTexture->setWrap(osg::Texture::WRAP_R, osg::Texture::REPEAT);
                mWrappingTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
                mWrappingTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
            }

            return mWrappingTexture;
        }
    }


}
