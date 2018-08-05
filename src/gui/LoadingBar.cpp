/*
 * LoadingBar.cpp
 *
 *  Created on: 22 Jun 2018
 *      Author: zal
 */

#include "gui/LoadingBar.h"

#include "gui/GuiManager.h"
#include "gui/GuiTextures.h"
#include "db/Texture.h"

namespace od
{

    LoadingBar::LoadingBar(GuiManager &gm)
    : DrawableWidget(gm)
    , mProgress(0.0)
    , mForegroundQuad(new TexturedQuad)
    , mBarQuad(new TexturedQuad)
    {
        osg::ref_ptr<Texture> barTexture = gm.getTexture(GuiTextures::LoadingBar);

        osg::Vec2 foreTl(0, 0);
        osg::Vec2 foreBr(127, 50);
        mForegroundQuad->setTextureImage(barTexture);
        mForegroundQuad->setTextureCoordsFromPixels(foreTl, foreBr);
        mForegroundQuad->setVertexCoords(osg::Vec2(0.0, 0.0), osg::Vec2(1.0, 1.0));
        mForegroundQuad->setZCoord(0.1);
        this->addDrawable(mForegroundQuad);

        mBarQuad->setTextureImage(barTexture);
        mBarQuad->setTextureCoordsFromPixels(osg::Vec2(0, 51), osg::Vec2(mProgress*127, 80));
        mBarQuad->setVertexCoords(osg::Vec2(0.0, 9.0/50), osg::Vec2(mProgress, 40.0/50));
        mBarQuad->setZCoord(0.2);
        this->addDrawable(mBarQuad);

        this->setDimensions(128.0, 51.0, WidgetDimensionType::Pixels);
    }

    void LoadingBar::setProgress(float f)
    {
        mProgress = f;

        mBarQuad->setTextureCoordsFromPixels(osg::Vec2(0, 51), osg::Vec2(mProgress*127, 80));
        mBarQuad->setVertexCoords(osg::Vec2(0.0, 9.0/50), osg::Vec2(mProgress, 40.0/50));
    }

}
