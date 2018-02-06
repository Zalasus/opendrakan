/*
 * TextureAtlas.cpp
 *
 *  Created on: 5 Feb 2018
 *      Author: zal
 */

#include "TextureAtlas.h"

#include <cmath>

#include "Exception.h"
#include "Logger.h"

namespace od
{

    TextureAtlas::TextureAtlas()
    : mFinished(false)
    , mNewYPixelOffset(0)
    {
    }

    void TextureAtlas::addTexture(const AssetRef &textureRef, TexturePtr texture)
    {
        if(mFinished)
        {
            throw Exception("Can't add textures to atlas that has already been built");
        }

        if(mTextureMap.find(textureRef) != mTextureMap.end())
        {
            // already added. ignore
            return;
        }

        AtlasEntry entry;
        entry.texture = texture;
        mTextureMap[textureRef] = entry;
    }

    void TextureAtlas::build()
    {
        // this is where the magic happens

        if(mTextureMap.size() == 0)
        {
            mFinished = true;
            return;
        }

        // for testing purposes, build a 1x1 atlas with the last texture we find
        TexturePtr tex;
        for(std::pair<AssetRef, AtlasEntry> entry : mTextureMap)
        {
            entry.second.topleft = osg::Vec2(0, 0);
            entry.second.bottomright = osg::Vec2(1, 1);
            tex = entry.second.texture;
        }

        this->setImage(tex->s(), tex->t(), 1, tex->getInternalTextureFormat(), tex->getPixelFormat(), tex->getDataType(), tex->data(), osg::Image::NO_DELETE);

        mFinished = true;
    }

    std::pair<osg::Vec2, osg::Vec2> TextureAtlas::getUvOfTexture(const AssetRef &textureRef)
    {
        if(!mFinished)
        {
            throw Exception("Tried to access texture coordinates from not yet built atlas");
        }

        auto it = mTextureMap.find(textureRef);
        if(it == mTextureMap.end())
        {
            Logger::error() << "Requested texture " << textureRef << " from atlas, which was not found in atlas";
            throw NotFoundException("Given texture was not added to texture atlas");
        }

        return std::pair<osg::Vec2, osg::Vec2>(it->second.topleft, it->second.bottomright);
    }

}
