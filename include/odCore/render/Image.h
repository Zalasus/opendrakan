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
        Model
    };

    /**
     * Interface for encapsulating texture pixel storage.
     *
     * This allows for multiple textures to share the same pixel data.
     */
    class Image : public od::RefCounted
    {
    public:

        /**
         * @brief Returns UV image dimensions. W dimension (depth for 3D textures) is ignored.
         */
        virtual glm::vec2 getDimensionsUV() = 0;

        /**
         * Creates a new texture (encapsulating sample state, not texel state) for use with the
         * rendering system. Note that this will create a new texture everytime.
         */
        virtual od::RefPtr<odRender::Texture> createTexture() = 0;

        /**
         * @brief Creates or returns a previously created texture configured for a specific use case.
         *
         * This will not create a new texture if one has already been created for the given use case.
         */
        virtual od::RefPtr<odRender::Texture> getTextureForUsage(TextureUsage usage) = 0;


    };

}


#endif /* INCLUDE_ODCORE_RENDER_IMAGE_H_ */
