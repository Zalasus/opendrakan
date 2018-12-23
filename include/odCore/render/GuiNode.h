/*
 * GuiNode.h
 *
 *  Created on: 17 Dec 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_GUINODE_H_
#define INCLUDE_ODCORE_RENDER_GUINODE_H_

#include <glm/mat4x4.hpp>

#include <odCore/RefCounted.h>

namespace odRender
{
    class GuiQuad;

    class GuiNode : public od::RefCounted
    {
    public:

        virtual void addChild(GuiNode *node) = 0;
        virtual void removeChild(GuiNode *node) = 0;

        virtual void setMatrix(const glm::mat4 &matrix) = 0;

        virtual void setVisible(bool visible) = 0;

        virtual GuiQuad *createGuiQuad() = 0;
        virtual void removeGuiQuad(GuiQuad *quad) = 0;

    };

}

#endif /* INCLUDE_ODCORE_RENDER_GUINODE_H_ */
