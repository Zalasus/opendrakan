/*
 * Texture.h
 *
 *  Created on: 23 Nov 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_TEXTURE_H_
#define INCLUDE_ODOSG_TEXTURE_H_

#include <osg/Image>

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


    private:

        od::RefPtr<odDb::Texture> mDbTexture;

        osg::ref_ptr<osg::Image> mImage;

    };

}

#endif /* INCLUDE_ODOSG_TEXTURE_H_ */
