/*
 * Texture.cpp
 *
 *  Created on: 23 Nov 2018
 *      Author: zal
 */

#include <odOsg/Texture.h>

#include <odCore/db/Texture.h>

#include <odOsg/Image.h>

namespace odOsg
{

    Texture::Texture(odOsg::Image *image)
    : mImage(image)
    {
        mTexture = new osg::Texture2D(mImage->getOsgImage());
    }

    Texture::~Texture()
    {
    }

    void Texture::setEnableWrapping(bool wrap)
    {
        setEnableWrapping(Dimension::U, wrap);
        setEnableWrapping(Dimension::V, wrap);
        setEnableWrapping(Dimension::W, wrap);
    }

    void Texture::setEnableWrapping(Dimension dimension, bool wrap)
    {
        osg::Texture::WrapParameter param;
        osg::Texture::WrapMode mode = wrap ? osg::Texture::REPEAT : osg::Texture::CLAMP_TO_EDGE;
        switch(dimension)
        {
        case odRender::Texture::Dimension::U:
            param = osg::Texture::WRAP_S;
            break;

        case odRender::Texture::Dimension::V:
            param = osg::Texture::WRAP_T;
            break;

        case odRender::Texture::Dimension::W:
            param = osg::Texture::WRAP_R;
            break;
        }

        mTexture->setWrap(param, mode);
    }

    odRender::Image *Texture::getImage()
    {
        return mImage;
    }

}
