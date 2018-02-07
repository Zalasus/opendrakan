/*
 * TextureAtlas.h
 *
 *  Created on: 5 Feb 2018
 *      Author: zal
 */

#ifndef TEXTUREATLAS_H_
#define TEXTUREATLAS_H_

#include <utility>
#include <map>
#include <vector>
#include <osg/Vec2>
#include <osg/Image>

#include "Texture.h"

namespace od
{

    class TextureAtlas : public osg::Image
    {
    public:

        TextureAtlas();

        void addTexture(const AssetRef &ref, TexturePtr texture);
        void build();

        std::pair<osg::Vec2, osg::Vec2> getUvOfTexture(const AssetRef &textureRef);

        void exportToPng(const std::string &path);


    private:

        struct AtlasEntry
        {
            TexturePtr texture;
            osg::Vec2 topleft;
            osg::Vec2 bottomright;
            size_t pixelY;
        };

        bool mFinished;
        std::map<AssetRef, AtlasEntry> mTextureMap;
    };

}

#endif /* TEXTUREATLAS_H_ */
