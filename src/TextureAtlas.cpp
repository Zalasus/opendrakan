/*
 * TextureAtlas.cpp
 *
 *  Created on: 5 Feb 2018
 *      Author: zal
 */

#include "TextureAtlas.h"

#include <cmath>
#include <algorithm>

#include "Exception.h"
#include "Logger.h"

namespace od
{

    TextureAtlas::TextureAtlas()
    : mFinished(false)
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

        // for simplicity, build a 1x* atlas
        int atlasSizeX = 0;
        int atlasSizeY = 0;
        GLint bytePerPixel = 0;
        size_t nextYOffset = 0;
        for(std::pair<AssetRef, AtlasEntry> entry : mTextureMap)
        {
            atlasSizeX = std::max(atlasSizeX, entry.second.texture->s());
            bytePerPixel = std::max(bytePerPixel, entry.second.texture->getInternalTextureFormat());

            entry.second.pixelY = nextYOffset;
            nextYOffset += entry.second.texture->t();
        }
        atlasSizeY = nextYOffset;

        mPixelBuffer.resize(atlasSizeX*atlasSizeY*bytePerPixel);

        for(std::pair<AssetRef, AtlasEntry> entry : mTextureMap)
        {
            // copy texture row by row
            unsigned char *atlasStart = mPixelBuffer.data() + entry.second.pixelY*atlasSizeX*bytePerPixel;

            for(int row = 0; row < entry.second.texture->t(); ++row)
            {
                unsigned char *texStart = entry.second.texture->data() + entry.second.texture->getRowStepInBytes()*row;

                memcpy(atlasStart+row*atlasSizeX, texStart, entry.second.texture->getRowSizeInBytes());
            }

            float tv = static_cast<float>(entry.second.pixelY) / atlasSizeY;
            float bu = static_cast<float>(entry.second.texture->s()) / atlasSizeX;
            float bv = static_cast<float>(entry.second.pixelY + entry.second.texture->t()) / atlasSizeY;

            entry.second.topleft = osg::Vec2(0, tv);
            entry.second.bottomright = osg::Vec2(bu, bv);
        }

        this->setImage(atlasSizeX, nextYOffset, 1, 4, GL_RGBA, GL_UNSIGNED_BYTE, mPixelBuffer.data(), osg::Image::NO_DELETE);

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
