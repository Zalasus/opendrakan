/*
 * Cursor.cpp
 *
 *  Created on: 30 Jun 2018
 *      Author: zal
 */

#include "gui/Cursor.h"

#include "gui/GuiManager.h"

namespace od
{

    Cursor::Cursor(GuiManager &gm)
    {
        osg::ref_ptr<TexturedQuad> cursorQuad = new TexturedQuad;
        cursorQuad->setTextureImage(gm.getTexture(0x16));

        // for some reason, the cursor image is offset left by 2 pixels with the pixels wrapping
        //  over to the right. maybe this is due to some strange way in which they implemented the
        //  cursor anchor? we fix this by using repeat mode for the U coordinate.
        cursorQuad->getTexture()->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
        cursorQuad->setTextureCoordsFromPixels(osg::Vec2(-2.5, 0), osg::Vec2(29.5, 32));

        cursorQuad->setVertexCoords(osg::Vec2(0.0, 0.0), osg::Vec2(1, 1));
        this->addGeometry(cursorQuad);
    }

    osg::Vec2 Cursor::getFullScaleDimensions() const
    {
        return osg::Vec2(32, 32);
    }

}
