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
        cursorQuad->setTexture(gm.getTexture(0x16));
        cursorQuad->setTextureCoordsFromPixels(osg::Vec2(0, 0), osg::Vec2(21, 29));
        cursorQuad->setVertexCoords(osg::Vec2(0.0, 0.0), osg::Vec2(1, 1));
        this->addGeometry(cursorQuad);
    }

    osg::Vec2 Cursor::getFullScaleDimensions() const
    {
        return osg::Vec2(22, 30);
    }

}
