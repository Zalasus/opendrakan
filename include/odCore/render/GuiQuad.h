/*
 * GuiQuad.h
 *
 *  Created on: 11 Dec 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_GUIQUAD_H_
#define INCLUDE_ODCORE_RENDER_GUIQUAD_H_

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <odCore/RefCounted.h>

#include <odCore/db/Asset.h>

namespace odRender
{
    class Texture;
    class Renderer;

    class GuiQuad : public od::RefCounted
    {
    public:

        virtual odRender::Texture *getTexture() = 0;

        /**
         * Convenience method. Fetches texture from an asset provider, sets up image and creates
         * a new render texture from a DB texture to be used by this quad.
         */
        void setTextureFromDb(odDb::AssetProvider &ap, const odDb::AssetRef &textureRef, odRender::Renderer &renderer);
        void setTextureFromDb(odDb::AssetProvider &ap, od::RecordId textureId, odRender::Renderer &renderer)
        {
            setTextureFromDb(ap, odDb::AssetRef(textureId, 0), renderer);
        }

        virtual void setTexture(odRender::Texture *texture) = 0;
        virtual void setTextureCoords(const glm::vec2 &topLeft, const glm::vec2 &bottomRight) = 0;
        virtual void setTextureCoordsFromPixels(const glm::vec2 &topLeft, const glm::vec2 &bottomRight) = 0;
        virtual void setVertexCoords(const glm::vec2 &topLeft, const glm::vec2 &bottomRight) = 0;

        virtual void setColor(const glm::vec4 &color) = 0;

    };

}

#endif /* INCLUDE_ODCORE_RENDER_GUIQUAD_H_ */
