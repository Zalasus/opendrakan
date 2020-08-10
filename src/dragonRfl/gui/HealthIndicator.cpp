/*
 * HealthIndicator.cpp
 *
 *  Created on: Aug 26, 2019
 *      Author: zal
 */

#include <dragonRfl/gui/HealthIndicator.h>

#include <cmath>

#include <glm/common.hpp>

#include <odCore/gui/Quad.h>
#include <odCore/render/Texture.h>

#include <dragonRfl/gui/DragonGui.h>
#include <dragonRfl/gui/GuiTextures.h>

namespace dragonRfl
{

    class Orb : public odGui::Widget
    {
    public:

        Orb(DragonGui &gui, od::RecordId bubbleTexture, const glm::vec2 &topLeft, const glm::vec2 &bottomRight, bool flip)
        : odGui::Widget(gui)
        , mTopLeftPx_Left(topLeft)
        , mBottomRightPx_Left(bottomRight)
        , mHeightPx(glm::abs(mTopLeftPx_Left.y - mBottomRightPx_Left.y))
        , mOrbQuad_Left(gui.getRenderer())
        {
            float width = glm::abs(mTopLeftPx_Left.x - mBottomRightPx_Left.x);

            if(flip)
            {
                mTopLeftPx_Left.x += width;
                mBottomRightPx_Left.x -= width;
            }

            mOrbQuad_Left.setTextureFromDb(gui, bubbleTexture, gui.getRenderer());
            mOrbQuad_Left.getTexture()->setEnableWrapping(false);
            this->addRenderHandle(mOrbQuad_Left.getHandle());

            setFillRatio(1.0);

            this->setDimensions({ width, mHeightPx }, odGui::WidgetDimensionType::Pixels);
        }

        Orb(DragonGui &gui, od::RecordId bubbleTexture, const glm::vec2 &topLeft1, const glm::vec2 &bottomRight1
                , const glm::vec2 &topLeft2, const glm::vec2 &bottomRight2, bool flip)
        : odGui::Widget(gui)
        , mTopLeftPx_Left(topLeft1)
        , mBottomRightPx_Left(bottomRight1)
        , mTopLeftPx_Right(topLeft2)
        , mBottomRightPx_Right(bottomRight2)
        , mHeightPx(glm::abs(mTopLeftPx_Left.y - mBottomRightPx_Left.y))
        , mOrbQuad_Left(gui.getRenderer())
        , mOrbQuad_Right(gui.getRenderer())
        {
            if(flip)
            {
                float lwidth = glm::abs(mTopLeftPx_Left.x - mBottomRightPx_Left.x);
                mTopLeftPx_Left.x += lwidth;
                mBottomRightPx_Left.x -= lwidth;

                float rwidth = glm::abs(mTopLeftPx_Right.x - mBottomRightPx_Right.x);
                mTopLeftPx_Right.x += rwidth;
                mBottomRightPx_Right.x -= rwidth;

                std::swap(mTopLeftPx_Left, mTopLeftPx_Right);
                std::swap(mBottomRightPx_Left, mBottomRightPx_Right);
            }

            mOrbQuad_Left.setTextureFromDb(gui, bubbleTexture, gui.getRenderer());
            mOrbQuad_Left.getTexture()->setEnableWrapping(false);
            this->addRenderHandle(mOrbQuad_Left.getHandle());

            mOrbQuad_Right.setTextureFromDb(gui, bubbleTexture, gui.getRenderer());
            mOrbQuad_Right.getTexture()->setEnableWrapping(false);
            this->addRenderHandle(mOrbQuad_Right.getHandle());

            setFillRatio(1.0);

            float width = glm::abs(mTopLeftPx_Left.x - mBottomRightPx_Left.x) + glm::abs(mTopLeftPx_Right.x - mBottomRightPx_Right.x);
            this->setDimensions(glm::vec2(width, mHeightPx), odGui::WidgetDimensionType::Pixels);
        }

        void setFillRatio(float ratio)
        {
            ratio = glm::clamp(ratio, 0.0f, 1.0f);

            float invRatio = 1 - ratio;
            glm::vec2 topOffset(0, mHeightPx*invRatio);

            if(!mOrbQuad_Left.empty())
            {
                mOrbQuad_Left.setTextureCoordsFromPixels(mTopLeftPx_Left+topOffset, mBottomRightPx_Left);
                if(mOrbQuad_Right.empty())
                {
                    mOrbQuad_Left.setVertexCoords(glm::vec2(0, invRatio), glm::vec2(1,1));

                }else
                {
                    mOrbQuad_Left.setVertexCoords(glm::vec2(0, invRatio), glm::vec2(0.5,1));
                }
            }

            if(!mOrbQuad_Right.empty())
            {
                mOrbQuad_Right.setTextureCoordsFromPixels(mTopLeftPx_Right+topOffset, mBottomRightPx_Right);
                if(mOrbQuad_Left.empty())
                {
                    mOrbQuad_Right.setVertexCoords(glm::vec2(0, invRatio), glm::vec2(1,1));

                }else
                {
                    mOrbQuad_Right.setVertexCoords(glm::vec2(0.5, invRatio), glm::vec2(1,1));
                }
            }
        }

        void setColor(const glm::vec4 &color)
        {
            if(!mOrbQuad_Left.empty())
            {
                mOrbQuad_Left.setColor(color);
            }

            if(!mOrbQuad_Right.empty())
            {
                mOrbQuad_Left.setColor(color);
            }
        }


    private:

        glm::vec2 mTopLeftPx_Left;
        glm::vec2 mBottomRightPx_Left;

        glm::vec2 mTopLeftPx_Right;
        glm::vec2 mBottomRightPx_Right;

        float mHeightPx;

        odGui::Quad mOrbQuad_Left;
        odGui::Quad mOrbQuad_Right;
    };


    HealthIndicator::HealthIndicator(DragonGui &gui)
    : odGui::Widget(gui)
    , mDragonGui(gui)
    , mHealthLevel(1.0)
    {
        glm::vec2 borderTL(0, 130);
        glm::vec2 borderBR(95, 220);
        glm::vec2 size = borderBR - borderTL;

        this->setDimensions(size, odGui::WidgetDimensionType::Pixels);

        odGui::Quad border(gui.getRenderer());
        border.setTextureFromDb(gui, GuiTextures::HudElements, gui.getRenderer());
        border.setTextureCoordsFromPixels(glm::vec2(0, 130), glm::vec2(95, 220));
        border.setVertexCoords(glm::vec2(0, 0), glm::vec2(1, 1));
        this->addRenderHandle(border.getHandle());

        glm::vec2 orbOffset(10, 13);

        // the health orb texture is split across two regions
        glm::vec2 healthTL_right(0,0);
        glm::vec2 healthBR_right(36, 63);
        glm::vec2 healthTL_left(93, 65);
        glm::vec2 healthBR_left(128, 127);
        mHealthOrb = std::make_shared<Orb>(gui, GuiTextures::HudOrbs_1, healthTL_left, healthBR_left, healthTL_right, healthBR_right, true);
        mHealthOrb->setZPosition(1.0);
        mHealthOrb->setPosition(orbOffset/size);
        this->addChild(mHealthOrb);

        glm::vec2 flashTL(122, 142);
        glm::vec2 flashBR(193, 205);
        mFlashOrb = std::make_shared<Orb>(gui, GuiTextures::HudElements, flashTL, flashBR, true);
        mFlashOrb->setZPosition(0.5);
        mFlashOrb->setPosition(orbOffset/size);
        this->addChild(mFlashOrb);

        mHealthLevel.move(0.8, 0.8);
        mHealthOrb->setFillRatio(mHealthLevel);
        mFlashOrb->setFillRatio(mHealthLevel);

        this->setEnableUpdate(true);
    }

    HealthIndicator::~HealthIndicator()
    {
    }

    void HealthIndicator::onUpdate(float relTime)
    {
        if(mHealthLevel.update(relTime))
        {
            mHealthOrb->setFillRatio(mHealthLevel);
            mFlashOrb->setFillRatio(mHealthLevel);

            float f = glm::clamp(mHealthLevel.normVelocity(), 0.0f, 1.0f);
            mFlashOrb->setColor(glm::vec4(1,1,1,f));
        }
    }

    void HealthIndicator::setFillState(float state, float animTime)
    {
        state = glm::clamp(state, 0.0f, 1.0f);

        if(animTime <= 0)
        {
            mHealthLevel.set(state);

        }else
        {
            mHealthLevel.move(state, animTime);
        }
    }

}
