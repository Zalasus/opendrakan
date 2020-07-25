/*
 * Image.h
 *
 *  Created on: Dec 12, 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_IMAGE_H_
#define INCLUDE_ODOSG_IMAGE_H_

#include <memory>
#include <osg/Image>

#include <odCore/render/Image.h>
#include <odCore/render/Texture.h>

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

        explicit Image(std::shared_ptr<odDb::Texture> dbTexture);
        virtual ~Image();

        inline osg::Image *getOsgImage() { return mOsgImage; }

        inline std::weak_ptr<odRender::Texture> getSharedObjectTexture() { return mSharedObjectTexture; }
        inline std::weak_ptr<odRender::Texture> getSharedLayerTexture() { return mSharedLayerTexture; }

        virtual glm::vec2 getDimensionsUV() override;


    private:

        std::shared_ptr<odDb::Texture> mDbTexture;
        std::weak_ptr<odRender::Texture> mSharedObjectTexture;
        std::weak_ptr<odRender::Texture> mSharedLayerTexture;

        osg::ref_ptr<osg::Image> mOsgImage;

    };

}


#endif /* INCLUDE_ODOSG_IMAGE_H_ */
