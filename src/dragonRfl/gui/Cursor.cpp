/*
 * Cursor.cpp
 *
 *  Created on: 30 Jun 2018
 *      Author: zal
 */

#include <dragonRfl/gui/Cursor.h>

#include <odCore/gui/GuiManager.h>
#include <dragonRfl/gui/GuiTextures.h>

namespace dragonRfl
{

    Cursor::Cursor(odGui::GuiManager &gm)
    : odGui::Widget(gm)
    {
        osg::ref_ptr<odGui::TexturedQuad> cursorQuad = new odGui::TexturedQuad;
        cursorQuad->setTextureImage(gm.getAsset<odDb::Texture>(GuiTextures::Cursor));

        // for some reason, the cursor image is offset left by 2 pixels with the pixels wrapping
        //  over to the right. maybe this is due to some strange way in which they implemented the
        //  cursor anchor? we fix this by using repeat mode for the U coordinate.
        cursorQuad->getTexture()->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
        cursorQuad->setTextureCoordsFromPixels(osg::Vec2(-2.5, 0), osg::Vec2(29.5, 32));

        cursorQuad->setVertexCoords(osg::Vec2(0.0, 0.0), osg::Vec2(1, 1));
        this->addDrawable(cursorQuad);

        this->setOrigin(odGui::WidgetOrigin::TopLeft);
        this->setDimensions(32.0, 32.0, odGui::WidgetDimensionType::Pixels);
    }

}
