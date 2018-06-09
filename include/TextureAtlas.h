/*
 * TextureAtlas.h
 *
 *  Created on: 5 Feb 2018
 *      Author: zal
 */

#ifndef TEXTUREATLAS_H_
#define TEXTUREATLAS_H_

#include <tuple>
#include <map>
#include <vector>
#include <osg/Vec2>
#include <osg/Image>

#include "db/Texture.h"

namespace od
{

    class TextureAtlas : public osg::Image
    {
    public:

        TextureAtlas();

        /**
         * @brief Registers a texture to be used in the atlas. Call will be ignored if texture is already registered.
         *
         * Once the atlas has been built, this must not be called anymore or the method will throw.
         */
        void addTexture(const AssetRef &ref, osg::ref_ptr<Texture> texture);
        void build();

        /**
         * @brief Get UV coordinates of texture as tuple of top-left, top-right, bottom-left and bottom-right coordinates.
         */
        std::tuple<osg::Vec2, osg::Vec2, osg::Vec2, osg::Vec2> getUvOfTexture(const AssetRef &textureRef);

        void exportToPng(const std::string &path);

        // override osg::Group
		virtual const char *libraryName() const override { return "od";    }
        virtual const char *className()   const override { return "TextureAtlas"; }


    private:

        struct AtlasEntry
        {
            osg::ref_ptr<Texture> texture;
            osg::Vec2 uvA;
            osg::Vec2 uvB;
            osg::Vec2 uvC;
            osg::Vec2 uvD;
            size_t pixelY;
        };

        bool mFinished;
        std::map<AssetRef, AtlasEntry> mTextureMap;
    };

}

#endif /* TEXTUREATLAS_H_ */
