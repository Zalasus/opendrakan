/*
 * MainMenu.cpp
 *
 *  Created on: 29 Jun 2018
 *      Author: zal
 */

#include "gui/MainMenu.h"

#include "gui/GuiManager.h"
#include "gui/TexturedQuad.h"

namespace od
{

    MainMenu::MainMenu(GuiManager &gm)
    {
        osg::ref_ptr<TexturedQuad> topLeft = new TexturedQuad;
        topLeft->setTextureImage(gm.getTexture(0x1410));
        topLeft->setTextureCoordsFromPixels(osg::Vec2(0, 0), osg::Vec2(255, 255));
        topLeft->setVertexCoords(osg::Vec2(0.0, 0.0), osg::Vec2(0.5, 0.5));
        this->addGeometry(topLeft);

        osg::ref_ptr<TexturedQuad> topRight = new TexturedQuad;
        topRight->setTextureImage(gm.getTexture(0x1411));
        topRight->setTextureCoordsFromPixels(osg::Vec2(0, 0), osg::Vec2(255, 255));
        topRight->setVertexCoords(osg::Vec2(0.5, 0.0), osg::Vec2(1, 0.5));
        this->addGeometry(topRight);

        osg::ref_ptr<TexturedQuad> bottomLeft = new TexturedQuad;
        bottomLeft->setTextureImage(gm.getTexture(0x1412));
        bottomLeft->setTextureCoordsFromPixels(osg::Vec2(0, 0), osg::Vec2(255, 255));
        bottomLeft->setVertexCoords(osg::Vec2(0.0, 0.5), osg::Vec2(0.5, 1));
        this->addGeometry(bottomLeft);

        osg::ref_ptr<TexturedQuad> bottomRight = new TexturedQuad;
        bottomRight->setTextureImage(gm.getTexture(0x1413));
        bottomRight->setTextureCoordsFromPixels(osg::Vec2(0, 0), osg::Vec2(255, 255));
        bottomRight->setVertexCoords(osg::Vec2(0.5, 0.5), osg::Vec2(1.0, 1.0));
        this->addGeometry(bottomRight);
    }

    osg::Vec2 MainMenu::getFullScaleDimensions() const
    {
        return osg::Vec2(512.0, 512.0);
    }

}
