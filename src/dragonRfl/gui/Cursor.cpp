/*
 * Cursor.cpp
 *
 *  Created on: 30 Jun 2018
 *      Author: zal
 */

#include <dragonRfl/gui/Cursor.h>

#include <odCore/render/Texture.h>

#include <odCore/gui/Quad.h>

#include <dragonRfl/gui/GuiTextures.h>
#include <dragonRfl/gui/DragonGui.h>

namespace dragonRfl
{

    Cursor::Cursor(DragonGui &gui)
    : Widget(gui)
    {
        // the quad never changes after here and the widget shares ownership over the handle.
        //  thus we can put it on the stack and let it die after the constructor
        auto quad = gui.makeQuadFromGuiTexture(GuiTextures::Cursor);
        quad.setVertexCoords(glm::vec2(0.0, 0.0), glm::vec2(1, 1));

        // for some reason, the cursor image is offset left by 2 pixels with the pixels wrapping
        //  over to the right. maybe this is due to some strange way in which they implemented the
        //  cursor anchor? we fix this by using repeat mode for the U coordinate.
        quad.getTexture()->setEnableWrapping(odRender::Texture::Dimension::U, true);
        quad.setTextureCoordsFromPixels(glm::vec2(-2.5, 0), glm::vec2(29.5, 32));

        this->addRenderHandle(quad.getHandle());

        this->setOrigin(odGui::WidgetOrigin::TopLeft);
        this->setDimensions({32.0, 32.0}, odGui::WidgetDimensionType::Pixels);
    }

    bool Cursor::liesWithinLogicalArea(const glm::vec2 &pos)
    {
        // this causes the cursor widget to be excluded from any mouse events
        return false;
    }

}
