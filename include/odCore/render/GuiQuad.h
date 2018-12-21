/*
 * GuiQuad.h
 *
 *  Created on: 11 Dec 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_GUIQUAD_H_
#define INCLUDE_ODCORE_RENDER_GUIQUAD_H_

#include <glm/vec2.hpp>

#include <odCore/RefCounted.h>

namespace odRender
{
    class Texture;

    class GuiQuad : public od::RefCounted
    {
    public:

        virtual void setTexture(odRender::Texture *texture) = 0;
        virtual void setTextureCoords(const glm::vec2 &topLeft, const glm::vec2 &bottomRight) = 0;
        virtual void setTextureCoordsFromPixels(const glm::vec2 &topLeft, const glm::vec2 &bottomRight) = 0;
        virtual void setVertexCoords(const glm::vec2 &topLeft, const glm::vec2 &bottomRight) = 0;

    };

}

#endif /* INCLUDE_ODCORE_RENDER_GUIQUAD_H_ */
