/*
 * Texture.h
 *
 *  Created on: 23 Nov 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_TEXTURE_H_
#define INCLUDE_ODOSG_TEXTURE_H_

#include <osg/Texture2D>

#include <odCore/render/Texture.h>

namespace odOsg
{
    class Image;

    class Texture : public odRender::Texture
    {
    public:

        explicit Texture(std::shared_ptr<Image> image);
        virtual ~Texture();

        inline osg::Texture2D *getOsgTexture() { return mTexture; }

        virtual void setEnableWrapping(bool wrap) override;
        virtual void setEnableWrapping(Dimension dimension, bool wrap) override;
        virtual odRender::Image *getImage() override;


    private:

        std::shared_ptr<odOsg::Image> mImage;

        osg::ref_ptr<osg::Texture2D> mTexture;

    };

}

#endif /* INCLUDE_ODOSG_TEXTURE_H_ */
