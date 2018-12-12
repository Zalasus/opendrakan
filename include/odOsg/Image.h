/*
 * Image.h
 *
 *  Created on: Dec 12, 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_IMAGE_H_
#define INCLUDE_ODOSG_IMAGE_H_

#include <osg/Image>

#include <odCore/render/Image.h>

namespace odDb
{
    class Texture;
}

namespace odOsg
{
    class Renderer;

    class Image : public odRender::Image
    {
    public:

        Image(odDb::Texture *dbTexture);
        virtual ~Image();

        inline osg::Image *getOsgImage() { return mOsgImage; }

        virtual void makePixelsUnique() override;


    private:

        od::RefPtr<odDb::Texture> mDbTexture;

        osg::ref_ptr<osg::Image> mOsgImage;

    };

}


#endif /* INCLUDE_ODOSG_IMAGE_H_ */
