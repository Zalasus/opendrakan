/*
 * Image.cpp
 *
 *  Created on: Dec 12, 2018
 *      Author: zal
 */

#include <odOsg/Image.h>

#include <odCore/Exception.h>

#include <odCore/db/Texture.h>

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

    void Image::makePixelsUnique()
    {
        throw od::UnsupportedException("makePixelsUnique() is unimplemented as of now");
    }

     glm::vec2 Image::getDimensionsUV()
     {
         return glm::vec2(mDbTexture->getWidth(), mDbTexture->getHeight());
     }

}


