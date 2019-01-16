/*
 * GuiNode.h
 *
 *  Created on: 17 Dec 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_GUINODE_H_
#define INCLUDE_ODCORE_RENDER_GUINODE_H_

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

#include <odCore/RefCounted.h>

namespace odRender
{
    class GuiQuad;
    class ObjectNode;

    class GuiNode : public od::RefCounted
    {
    public:

        virtual void setMatrix(const glm::mat4 &m) = 0;
        virtual void setViewport(const glm::vec2 &offset, const glm::vec2 &size) = 0;

        virtual void setOrthogonalMode() = 0;
        virtual void setPerspectiveMode(float fov, float aspect) = 0;

        virtual void setVisible(bool visible) = 0;

        virtual void setZIndex(int32_t zIndex) = 0;
        virtual void reorderChildren() = 0;

        virtual GuiQuad *createGuiQuad() = 0;
        virtual void removeGuiQuad(GuiQuad *quad) = 0;

        virtual ObjectNode *createObjectNode() = 0;
        virtual void removeObjectNode(ObjectNode *node) = 0;

    };

}

#endif /* INCLUDE_ODCORE_RENDER_GUINODE_H_ */
