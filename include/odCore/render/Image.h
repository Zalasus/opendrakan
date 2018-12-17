/*
 * Image.h
 *
 *  Created on: Dec 12, 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_IMAGE_H_
#define INCLUDE_ODCORE_RENDER_IMAGE_H_

#include <glm/vec2.hpp>

#include <odCore/RefCounted.h>

namespace odRender
{

    class Texture;

    enum class TextureUsage
    {
        Layer,
        Model,
        Custom
    };

    /**
     * Interface for encapsulating texture pixel storage.
     *
     * This allows for multiple textures to share the same pixel data.
     */
    class Image : public od::RefCounted
    {
    public:

        virtual void makePixelsUnique() = 0;
        virtual glm::vec2 getDimensionsUV() = 0;

        virtual odRender::Texture *createTexture() = 0;
        virtual odRender::Texture *getTextureForUsage(TextureUsage usage) = 0;


    };

}


#endif /* INCLUDE_ODCORE_RENDER_IMAGE_H_ */
