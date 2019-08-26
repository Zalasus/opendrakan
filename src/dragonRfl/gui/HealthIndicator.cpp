/*
 * HealthIndicator.cpp
 *
 *  Created on: Aug 26, 2019
 *      Author: zal
 */

#include <dragonRfl/gui/HealthIndicator.h>

#include <glm/common.hpp>

#include <odCore/render/GuiNode.h>

#include <dragonRfl/gui/DragonGui.h>
#include <dragonRfl/gui/GuiTextures.h>

namespace dragonRfl
{

    class Bubble : public odGui::Widget
    {
    public:

        Bubble(DragonGui &gui, od::RecordId bubbleTexture, const glm::vec2 &topLeft, const glm::vec2 &bottomRight)
        : odGui::Widget(gui)
        , mTopLeftPx_Left(topLeft)
        , mBottomRightPx_Left(bottomRight)
        , mHeightPx(glm::abs(mTopLeftPx_Left.y - mBottomRightPx_Left.y))
        {
            mBubbleQuad_Left = this->getRenderNode()->createGuiQuad();
            mBubbleQuad_Left->setTextureFromDb(gui, bubbleTexture, gui.getRenderer());
            setFillRatio(1.0);

            float width = glm::abs(mTopLeftPx_Left.x - mBottomRightPx_Left.x);
            this->setDimensions(glm::vec2(width, mHeightPx), odGui::WidgetDimensionType::Pixels);
        }

        Bubble(DragonGui &gui, od::RecordId bubbleTexture, const glm::vec2 &topLeft1, const glm::vec2 &bottomRight1
                , const glm::vec2 &topLeft2, const glm::vec2 &bottomRight2)
        : odGui::Widget(gui)
        , mTopLeftPx_Left(topLeft1)
        , mBottomRightPx_Left(bottomRight1)
        , mTopLeftPx_Right(topLeft2)
        , mBottomRightPx_Right(bottomRight2)
        , mHeightPx(glm::abs(mTopLeftPx_Left.y - mBottomRightPx_Left.y))
        {
            mBubbleQuad_Left = this->getRenderNode()->createGuiQuad();
            mBubbleQuad_Left->setTextureFromDb(gui, bubbleTexture, gui.getRenderer());
            mBubbleQuad_Right = this->getRenderNode()->createGuiQuad();
            mBubbleQuad_Right->setTextureFromDb(gui, bubbleTexture, gui.getRenderer());
            setFillRatio(1.0);

            float width = glm::abs(mTopLeftPx_Left.x - mBottomRightPx_Left.x) + glm::abs(mTopLeftPx_Right.x - mBottomRightPx_Right.x);
            this->setDimensions(glm::vec2(width, mHeightPx), odGui::WidgetDimensionType::Pixels);
        }

        void flipHorizontally()
        {

        }

        void setFillRatio(float ratio)
        {
            ratio = glm::clamp(ratio, 0.0f, 1.0f);

            float invRatio = 1 - ratio;
            glm::vec2 topOffset(0, mHeightPx*invRatio);

            if(mBubbleQuad_Left != nullptr)
            {
                mBubbleQuad_Left->setTextureCoordsFromPixels(mTopLeftPx_Left+topOffset, mBottomRightPx_Left);
                if(mBubbleQuad_Right == nullptr)
                {
                    mBubbleQuad_Left->setVertexCoords(glm::vec2(0, invRatio), glm::vec2(1,1));

                }else
                {
                    mBubbleQuad_Left->setVertexCoords(glm::vec2(0, invRatio), glm::vec2(0.5,1));
                }
            }

            if(mBubbleQuad_Right != nullptr)
            {
                mBubbleQuad_Right->setTextureCoordsFromPixels(mTopLeftPx_Right+topOffset, mBottomRightPx_Right);
                if(mBubbleQuad_Left == nullptr)
                {
                    mBubbleQuad_Right->setVertexCoords(glm::vec2(0, invRatio), glm::vec2(1,1));

                }else
                {
                    mBubbleQuad_Right->setVertexCoords(glm::vec2(0.5, invRatio), glm::vec2(1,1));
                }
            }
        }


    private:

        od::RefPtr<odRender::GuiQuad> mBubbleQuad_Left;
        glm::vec2 mTopLeftPx_Left;
        glm::vec2 mBottomRightPx_Left;

        od::RefPtr<odRender::GuiQuad> mBubbleQuad_Right;
        glm::vec2 mTopLeftPx_Right;
        glm::vec2 mBottomRightPx_Right;

        float mHeightPx;
    };


    HealthIndicator::HealthIndicator(DragonGui &gui)
    : odGui::Widget(gui)
    , mDragonGui(gui)
    , mHealthLevel(1.0)
    {
        auto node = this->getRenderNode();

        glm::vec2 borderTL(0, 130);
        glm::vec2 borderBR(95, 220);
        glm::vec2 size = borderBR - borderTL;

        this->setDimensions(size, odGui::WidgetDimensionType::Pixels);

        auto border = node->createGuiQuad();
        border->setTextureFromDb(gui, GuiTextures::HudElements, gui.getRenderer());
        border->setTextureCoordsFromPixels(glm::vec2(0, 130), glm::vec2(95, 220));
        border->setVertexCoords(glm::vec2(0, 0), glm::vec2(1, 1));

        glm::vec2 orbOffset(10, 13);

        // the health orb texture is split across two regions
        glm::vec2 healthTL_right(0,0);
        glm::vec2 healthBR_right(36, 63);
        glm::vec2 healthTL_left(93, 65);
        glm::vec2 healthBR_left(128, 127);
        mHealthBubble = od::make_refd<Bubble>(gui, GuiTextures::HudOrbs_1, healthTL_left, healthBR_left, healthTL_right, healthBR_right);
        mHealthBubble->flipHorizontally();
        mHealthBubble->setZIndex(2);
        mHealthBubble->setPosition(orbOffset/size);
        this->addChild(mHealthBubble);

        glm::vec2 flashTL(122, 142);
        glm::vec2 flashBR(193, 205);
        mFlashBubble = od::make_refd<Bubble>(gui, GuiTextures::HudElements, flashTL, flashBR);
        mFlashBubble->flipHorizontally();
        mFlashBubble->setZIndex(1);
        mFlashBubble->setPosition(orbOffset/size);
        this->addChild(mFlashBubble);

        mHealthLevel.move(0, 10);
        mHealthBubble->setFillRatio(mHealthLevel.get());
        mFlashBubble->setFillRatio(mHealthLevel.get());
    }

    HealthIndicator::~HealthIndicator()
    {
    }

    void HealthIndicator::onUpdate(float relTime)
    {
        if(mHealthLevel.update(relTime))
        {
            Logger::info() << mHealthLevel.get();
            mHealthBubble->setFillRatio(mHealthLevel.get());
            mFlashBubble->setFillRatio(mHealthLevel.get());
        }
    }

}


