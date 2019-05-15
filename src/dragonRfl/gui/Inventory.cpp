/*
 * Inventory.cpp
 *
 *  Created on: Apr 6, 2019
 *      Author: zal
 */

#include <dragonRfl/gui/Inventory.h>

#include <odCore/render/Renderer.h>
#include <odCore/render/GuiNode.h>
#include <odCore/render/GuiQuad.h>
#include <odCore/render/Image.h>
#include <odCore/render/Texture.h>

#include <dragonRfl/gui/DragonGui.h>
#include <dragonRfl/gui/GuiTextures.h>

namespace dragonRfl
{

    Inventory::Inventory(DragonGui &gui)
    : Widget(gui)
    {
        od::RefPtr<odRender::GuiQuad> invQuadLeft = this->getRenderNode()->createGuiQuad();
        invQuadLeft->setTextureFromDb(gui, GuiTextures::Inventory_Left, gui.getRenderer());

        od::RefPtr<odRender::GuiQuad> invQuadRight = this->getRenderNode()->createGuiQuad();
        invQuadRight->setTextureFromDb(gui, GuiTextures::Inventory_Right, gui.getRenderer());

        this->setOrigin(odGui::WidgetOrigin::TopLeft);
        this->setDimensions(32.0, 32.0, odGui::WidgetDimensionType::Pixels);

    }

}


