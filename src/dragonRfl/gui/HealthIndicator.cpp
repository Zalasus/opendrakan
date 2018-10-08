/*
 * HealthIndicator.cpp
 *
 *  Created on: Aug 7, 2018
 *      Author: zal
 */

#include <dragonRfl/gui/HealthIndicator.h>

#include <dragonRfl/gui/GuiTextures.h>
#include <odCore/gui/GuiManager.h>

namespace od
{

    HealthIndicator::HealthIndicator(GuiManager &gm)
    : Widget(gm)
    , mHealth(1.0)
    {
        osg::ref_ptr<TexturedQuad> bg(new TexturedQuad);
        bg->setTextureImage(gm.getAsset<Texture>(GuiTextures::HudElements));
        bg->setTextureCoordsFromPixels(osg::Vec2(0, 128), osg::Vec2(96, 128+96));
        bg->setVertexCoords(osg::Vec2(0, 0), osg::Vec2(1, 1));
        this->addDrawable(bg);

        mOrbFlashQuad = new TexturedQuad;
        mOrbFlashQuad->setTextureImage(gm.getAsset<Texture>(GuiTextures::HudElements));
        mOrbFlashQuad->setTextureCoordsFromPixels(osg::Vec2(122-10, 142-14), osg::Vec2(192+15, 204+19));
        mOrbFlashQuad->setVertexCoords(osg::Vec2(0, 0), osg::Vec2(1, 1));
        mOrbFlashQuad->setZCoord(1);
        this->addDrawable(mOrbFlashQuad);

        this->setDimensions(96, 96, WidgetDimensionType::Pixels);

    }

    void HealthIndicator::playFlashAnim()
    {

    }

    void HealthIndicator::setHealth(float health)
    {
        mHealth = health;
    }

}

