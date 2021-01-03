/*
 * Image.h
 *
 *  Created on: Dec 12, 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_IMAGE_H_
#define INCLUDE_ODCORE_RENDER_IMAGE_H_

#include <glm/vec2.hpp>

namespace odRender
{

    /**
     * Interface for encapsulating texture pixel storage.
     *
     * This allows for multiple textures to share the same pixel data.
     */
    class Image
    {
    public:

        virtual ~Image() = default;

        /**
         * @brief Returns UV image dimensions. W dimension (depth for 3D textures) is ignored.
         */
        virtual glm::vec2 getDimensionsUV() = 0;

    };

}


#endif /* INCLUDE_ODCORE_RENDER_IMAGE_H_ */
