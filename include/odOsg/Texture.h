/*
 * Texture.h
 *
 *  Created on: 23 Nov 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_TEXTURE_H_
#define INCLUDE_ODOSG_TEXTURE_H_

#include <osg/Image>
#include <osg/Texture2D>

#include <odCore/db/Texture.h>

#include <odCore/render/Texture.h>

namespace odOsg
{

    class Renderer;

    class Texture : public odRender::Texture
    {
    public:

        Texture(Renderer *renderer, odDb::Texture *dbTexture);
        virtual ~Texture();

        inline osg::Image *getImage() { return mImage; }

        osg::Texture2D *getOsgTexture(bool clamping);


    private:

        od::RefPtr<odDb::Texture> mDbTexture;

        osg::ref_ptr<osg::Image> mImage;
        osg::ref_ptr<osg::Texture2D> mWrappingTexture;
        osg::ref_ptr<osg::Texture2D> mClampingTexture;

    };

}

#endif /* INCLUDE_ODOSG_TEXTURE_H_ */
