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

    };

}


#endif /* INCLUDE_ODCORE_RENDER_IMAGE_H_ */
