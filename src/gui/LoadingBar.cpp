/*
 * LoadingBar.cpp
 *
 *  Created on: 22 Jun 2018
 *      Author: zal
 */

#include "gui/LoadingBar.h"

#include "gui/GuiManager.h"
#include "db/Texture.h"

namespace od
{

    LoadingBar::LoadingBar(GuiManager &gm)
    : mProgress(0.0)
    , mForegroundQuad(new TexturedQuad)
    , mBarQuad(new TexturedQuad)
    {
        osg::ref_ptr<Texture> barTexture = gm.getTexture(0x1400);

        osg::Vec2 foreTl(0, 0);
        osg::Vec2 foreBr(127, 50);
        mForegroundQuad->setTexture(barTexture);
        mForegroundQuad->setTextureCoordsFromPixels(foreTl, foreBr);
        mForegroundQuad->setVertexCoords(osg::Vec2(0.0, 0.0), osg::Vec2(1.0, 1.0));
        mForegroundQuad->setZCoord(-0.4);
        this->addGeometry(mForegroundQuad);

        mBarQuad->setTexture(barTexture);
        mBarQuad->setTextureCoordsFromPixels(osg::Vec2(0, 51), osg::Vec2(mProgress*127, 80));
        mBarQuad->setVertexCoords(osg::Vec2(0.0, 9.0/50), osg::Vec2(mProgress, 40.0/50));
        mBarQuad->setZCoord(-0.3);
        this->addGeometry(mBarQuad);
    }

    osg::Vec2 LoadingBar::getFullScaleDimensions() const
    {
        return osg::Vec2(128.0, 51.0);
    }

    void LoadingBar::setProgress(float f)
    {
        mProgress = f;

        mBarQuad->setTextureCoordsFromPixels(osg::Vec2(0, 51), osg::Vec2(mProgress*127, 80));
        mBarQuad->setVertexCoords(osg::Vec2(0.0, 9.0/50), osg::Vec2(mProgress, 40.0/50));
    }

}
