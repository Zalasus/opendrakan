/*
 * Texture.h
 *
 *  Created on: 23 Nov 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_TEXTURE_H_
#define INCLUDE_ODCORE_RENDER_TEXTURE_H_

#include <odCore/RefCounted.h>

namespace odRender
{
    class Image;

    class Texture : public od::RefCounted
    {
    public:

        enum class Dimension
        {
            U,
            V,
            W
        };

        virtual ~Texture() = default;

        virtual void setEnableWrapping(bool wrap) = 0;
        virtual void setEnableWrapping(Dimension dimension, bool wrap) = 0;
        virtual Image *getImage() = 0;
    };

}

#endif /* INCLUDE_ODCORE_RENDER_TEXTURE_H_ */
