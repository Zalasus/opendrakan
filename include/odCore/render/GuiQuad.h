/*
 * GuiQuad.h
 *
 *  Created on: 11 Dec 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_GUIQUAD_H_
#define INCLUDE_ODCORE_RENDER_GUIQUAD_H_

namespace odDb
{
    class Texture;
}

namespace odRender
{
    class Texture;

    class GuiQuad
    {
    public:

        virtual ~GuiQuad();

        virtual void setTexture(odDb::Texture *texture) = 0;
        virtual Texture *getRenderTexture() = 0;

        virtual void setTextureCoordsFromPixels(const glm::vec2 &topLeft, const glm::vec2 &topRight) = 0;

        virtual void setVertexCoords(const glm::vec2 &topLeft, const glm::vec2 &topRight) = 0;

    };

}

#endif /* INCLUDE_ODCORE_RENDER_GUIQUAD_H_ */
