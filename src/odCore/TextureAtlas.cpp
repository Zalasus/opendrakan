/*
 * TextureAtlas.cpp
 *
 *  Created on: 5 Feb 2018
 *      Author: zal
 */

#include <odCore/TextureAtlas.h>

#include <cmath>
#include <algorithm>
#include <osgDB/WriteFile>

#include <odCore/Exception.h>
#include <odCore/Logger.h>

namespace od
{

    TextureAtlas::TextureAtlas()
    : mFinished(false)
    {
    }

    void TextureAtlas::addTexture(const AssetRef &textureRef, osg::ref_ptr<Texture> texture)
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

    	Logger::debug() << "Building texture atlas";

        if(mTextureMap.size() == 0)
        {
            mFinished = true;
            return;
        }

        // for simplicity, build a 1x* atlas
        int atlasSizeX = 0;
        int atlasSizeY = 0;
        size_t nextYOffset = 0;
        for(auto it = mTextureMap.begin(); it != mTextureMap.end(); ++it)
        {
            atlasSizeX = std::max(atlasSizeX, it->second.texture->s());

            it->second.pixelY = nextYOffset;
            nextYOffset += it->second.texture->t();
        }
        atlasSizeY = nextYOffset;

        unsigned char *pixelBuffer = new unsigned char[atlasSizeX*atlasSizeY*4];
        this->setImage(atlasSizeX, atlasSizeY, 1, 4, GL_RGBA, GL_UNSIGNED_BYTE, pixelBuffer, osg::Image::USE_NEW_DELETE);

        for(auto it = mTextureMap.begin(); it != mTextureMap.end(); ++it)
        {
        	this->copySubImage(0, it->second.pixelY, 0, it->second.texture);

        	float au = 0.5 / atlasSizeX;
            float av = ((float)it->second.pixelY + 0.5) / atlasSizeY;
            float du = ((float)it->second.texture->s() + 0.5) / atlasSizeX;
            float dv = (((float)it->second.pixelY + 0.5) + it->second.texture->t()) / atlasSizeY;

            it->second.uvA = osg::Vec2(au, av);
            it->second.uvB = osg::Vec2(du, av);
            it->second.uvC = osg::Vec2(au, dv);
            it->second.uvD = osg::Vec2(du, dv);
        }

        mFinished = true;
    }

    std::tuple<osg::Vec2, osg::Vec2, osg::Vec2, osg::Vec2> TextureAtlas::getUvOfTexture(const AssetRef &textureRef)
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

        return std::tuple<osg::Vec2, osg::Vec2, osg::Vec2, osg::Vec2>(it->second.uvA, it->second.uvB, it->second.uvC, it->second.uvD);
    }

    void TextureAtlas::exportToPng(const std::string &path)
    {
    	osgDB::writeImageFile(*this, path);
    }

}
