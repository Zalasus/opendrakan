/*
 * Image.h
 *
 *  Created on: Dec 12, 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_IMAGE_H_
#define INCLUDE_ODOSG_IMAGE_H_

#include <osg/Image>

#include <odCore/WeakRefPtr.h>

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

        explicit Image(odDb::Texture *dbTexture);
        virtual ~Image();

        inline osg::Image *getOsgImage() { return mOsgImage; }

        virtual glm::vec2 getDimensionsUV() override;

        virtual od::RefPtr<odRender::Texture> createTexture() override;
        virtual od::RefPtr<odRender::Texture> getTextureForUsage(odRender::TextureUsage usage) override;


    private:

        od::RefPtr<odDb::Texture> mDbTexture;
        od::WeakObserverRefPtr<odRender::Texture> mModelRenderTexture;
        od::WeakObserverRefPtr<odRender::Texture> mLayerRenderTexture;

        osg::ref_ptr<osg::Image> mOsgImage;

    };

}


#endif /* INCLUDE_ODOSG_IMAGE_H_ */
