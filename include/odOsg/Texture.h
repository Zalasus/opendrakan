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

namespace odDb
{
    class Texture;
}

namespace odOsg
{

    class Texture : public odRender::Texture
    {
    public:

        Texture(odDb::Texture &dbTexture);

        inline osg::Image *getImage() { return mImage; }


    private:

        od::RefPtr<odDb::Texture> mDbTexture;

        osg::ref_ptr<osg::Image> mImage;

    };

}

#endif /* INCLUDE_ODOSG_TEXTURE_H_ */
