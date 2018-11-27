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

}
