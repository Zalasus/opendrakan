/*
 * GuiNode.h
 *
 *  Created on: 17 Dec 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_GUINODE_H_
#define INCLUDE_ODCORE_RENDER_GUINODE_H_

#include <memory>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

namespace odRender
{
    class GuiQuad;
    class Handle;

    class GuiNode
    {
    public:

        virtual void addChild(std::shared_ptr<GuiNode> node) = 0;
        virtual void removeChild(std::shared_ptr<GuiNode> node) = 0;

        virtual void setMatrix(const glm::mat4 &m) = 0;
        virtual void setViewport(const glm::vec2 &offset, const glm::vec2 &size) = 0;

        virtual void setOrthogonalMode() = 0;
        virtual void setPerspectiveMode(float fov, float aspect) = 0;

        virtual void setVisible(bool visible) = 0;

        virtual void setZIndex(int32_t zIndex) = 0;
        virtual void reorderChildren() = 0;

        virtual std::shared_ptr<GuiQuad> createGuiQuad() = 0;
        virtual void removeGuiQuad(std::shared_ptr<GuiQuad> quad) = 0;

        virtual void addChildHandle(std::shared_ptr<Handle> handle) = 0;
        virtual void removeChildHandle(std::shared_ptr<Handle> handle) = 0;

    };

}

#endif /* INCLUDE_ODCORE_RENDER_GUINODE_H_ */
