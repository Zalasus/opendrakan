/*
 * Cursor.cpp
 *
 *  Created on: 30 Jun 2018
 *      Author: zal
 */

#include <dragonRfl/gui/Cursor.h>

#include <odCore/render/Renderer.h>
#include <odCore/render/GuiQuad.h>
#include <odCore/render/Image.h>
#include <odCore/render/Texture.h>

#include <dragonRfl/gui/GuiTextures.h>
#include <dragonRfl/gui/DragonGui.h>

namespace dragonRfl
{

    Cursor::Cursor(DragonGui &gui)
    {
        od::RefPtr<odRender::GuiQuad> cursorQuad = gui.getRenderer()->createGuiQuad();
        od::RefPtr<odDb::Texture> cursorDbTexture = gui.getAsset<odDb::Texture>(GuiTextures::Cursor);
        od::RefPtr<odRender::Texture> texture = cursorDbTexture->getRenderImage(gui.getRenderer())->createTexture();
        cursorQuad->setTexture(texture);

        // for some reason, the cursor image is offset left by 2 pixels with the pixels wrapping
        //  over to the right. maybe this is due to some strange way in which they implemented the
        //  cursor anchor? we fix this by using repeat mode for the U coordinate.
        texture->setEnableWrapping(odRender::Texture::Dimension::U, true);
        cursorQuad->setTextureCoordsFromPixels(glm::vec2(-2.5, 0), glm::vec2(29.5, 32));

        cursorQuad->setVertexCoords(glm::vec2(0.0, 0.0), glm::vec2(1, 1));

        this->addDrawable(cursorQuad);

        this->setOrigin(odGui::WidgetOrigin::TopLeft);
        this->setDimensions(32.0, 32.0, odGui::WidgetDimensionType::Pixels);
    }

}
