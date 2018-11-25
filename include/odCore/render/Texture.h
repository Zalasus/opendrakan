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

    class Texture : public od::RefCounted
    {
    public:

        virtual ~Texture() = default;
    };

}

#endif /* INCLUDE_ODCORE_RENDER_TEXTURE_H_ */
