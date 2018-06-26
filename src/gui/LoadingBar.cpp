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

        osg::Vec2 foreTl(0.0, 0.0);
        osg::Vec2 foreBr(1.0, 50.0/128);
        mForegroundQuad->setTextureCoords(foreTl, foreBr);
        mForegroundQuad->setVertexCoords(osg::Vec2(0.0, 0.0), osg::Vec2(1.0, 1.0));
        mForegroundQuad->setZCoord(-0.4);
        mForegroundQuad->setTexture(barTexture);
        this->addGeometry(mForegroundQuad);

        mBarQuad->setTextureCoords(osg::Vec2(0.0, 51.0/128), osg::Vec2(mProgress, 80.0/128));
        mBarQuad->setVertexCoords(osg::Vec2(0.0, 9.0/50), osg::Vec2(mProgress, 40.0/50));
        mBarQuad->setZCoord(-0.5);
        mBarQuad->setTexture(barTexture);
        this->addGeometry(mBarQuad);
    }

    osg::Vec2 LoadingBar::getFullScaleDimensions() const
    {
        return osg::Vec2(128.0, 51.0);
    }

    void LoadingBar::setProgress(float f)
    {
        mProgress = f;

        mBarQuad->setTextureCoords(osg::Vec2(0.0, 51.0/128), osg::Vec2(mProgress, 80.0/128));
        mBarQuad->setVertexCoords(osg::Vec2(0.0, 9.0/50), osg::Vec2(mProgress, 40.0/50));
    }

}
