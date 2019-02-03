/*
 * MainMenu.cpp
 *
 *  Created on: 29 Jun 2018
 *      Author: zal
 */

#include <dragonRfl/gui/MainMenu.h>

#include <cmath>

#include <dragonRfl/gui/DragonGui.h>
#include <dragonRfl/gui/GuiTextures.h>
#include <dragonRfl/gui/CrystalRingButton.h>
#include <dragonRfl/classes/UserInterfaceProperties.h>

#include <odCore/db/Model.h>

#include <odCore/render/GuiNode.h>
#include <odCore/render/GuiQuad.h>

namespace dragonRfl
{

    class MainMenuImage : public odGui::Widget
    {
    public:

        MainMenuImage(DragonGui &gui)
        : odGui::Widget(gui)
        {
            auto node = getRenderNode();

            od::RefPtr<odRender::GuiQuad> topLeft = node->createGuiQuad();
            topLeft->setTextureFromDb(gui, GuiTextures::MainMenu_TopLeft, gui.getRenderer());
            topLeft->setTextureCoordsFromPixels(glm::vec2(0, 0), glm::vec2(255, 255));
            topLeft->setVertexCoords(glm::vec2(0.0, 0.0), glm::vec2(0.5, 0.5));

            od::RefPtr<odRender::GuiQuad> topRight = node->createGuiQuad();
            topRight->setTextureFromDb(gui, GuiTextures::MainMenu_TopRight, gui.getRenderer());
            topRight->setTextureCoordsFromPixels(glm::vec2(0, 0), glm::vec2(255, 255));
            topRight->setVertexCoords(glm::vec2(0.5, 0.0), glm::vec2(1, 0.5));

            od::RefPtr<odRender::GuiQuad> bottomLeft = node->createGuiQuad();
            bottomLeft->setTextureFromDb(gui, GuiTextures::MainMenu_BottomLeft, gui.getRenderer());
            bottomLeft->setTextureCoordsFromPixels(glm::vec2(0, 0), glm::vec2(255, 255));
            bottomLeft->setVertexCoords(glm::vec2(0.0, 0.5), glm::vec2(0.5, 1));

            od::RefPtr<odRender::GuiQuad> bottomRight = node->createGuiQuad();
            bottomRight->setTextureFromDb(gui, GuiTextures::MainMenu_BottomRight, gui.getRenderer());
            bottomRight->setTextureCoordsFromPixels(glm::vec2(0, 0), glm::vec2(255, 255));
            bottomRight->setVertexCoords(glm::vec2(0.5, 0.5), glm::vec2(1.0, 1.0));

            this->setDimensions(512.0, 512.0, odGui::WidgetDimensionType::Pixels);
        }

    };

    class MainMenuBackground : public odGui::Widget
    {
    public:

        MainMenuBackground(DragonGui &gui)
        : odGui::Widget(gui)
        {
            od::RefPtr<odRender::GuiQuad> bg = getRenderNode()->createGuiQuad();
            bg->setVertexCoords(glm::vec2(0.0, 0.0), glm::vec2(1.0, 1.0));
            bg->setColor(glm::vec4(0.0, 0.0, 0.0, 0.5));

            this->setDimensions(1.0, 1.0, odGui::WidgetDimensionType::ParentRelative);
        }
    };



    MainMenu::MainMenu(DragonGui &gui, UserInterfaceProperties *uiProps)
    : odGui::Widget(gui)
    , mDragonGui(gui)
    {
        auto imageWidget = od::make_refd<MainMenuImage>(gui);
        imageWidget->setZIndex(0);
        imageWidget->setOrigin(odGui::WidgetOrigin::Center);
        imageWidget->setPosition(glm::vec2(0.5, 0.5));
        this->addChild(imageWidget);

        auto bgWidget = od::make_refd<MainMenuBackground>(gui);
        bgWidget->setZIndex(1);
        this->addChild(bgWidget);

        this->setDimensions(1.0, 1.0, odGui::WidgetDimensionType::ParentRelative);

        auto cont = od::make_refd<Widget>(gui);
        cont->setDimensions(512, 512, odGui::WidgetDimensionType::Pixels);
        cont->setOrigin(odGui::WidgetOrigin::Center);
        cont->setPosition(0.5, 0.5);
        cont->setZIndex(-1);
        this->addChild(cont);

        _addCrystal(gui, uiProps->mCrystalTop.getAsset(), uiProps->mCrystalTopNoteOffset, 53, 255, 62, uiProps, cont, BC_MULTIPLAYER);
        _addCrystal(gui, uiProps->mCrystalLeft.getAsset(), uiProps->mCrystalLeftNoteOffset, 57, 110, 195, uiProps, cont, BC_LOAD);
        _addCrystal(gui, uiProps->mCrystalMiddle.getAsset(), uiProps->mCrystalMiddleNoteOffset, 67, 255, 191, uiProps, cont, BC_NEW);
        _addCrystal(gui, uiProps->mCrystalRight.getAsset(), uiProps->mCrystalRightNoteOffset, 57, 400, 195, uiProps, cont, BC_SAVE);
        _addCrystal(gui, uiProps->mCrystalLowerLeft.getAsset(), uiProps->mCrystalLowerLeftNoteOffset, 35, 152, 292, uiProps, cont, BC_OPTIONS);
        _addCrystal(gui, uiProps->mCrystalLowerRight.getAsset(), uiProps->mCrystalLowerRightNoteOffset, 35, 358, 292, uiProps, cont, BC_CREDITS);
        _addCrystal(gui, uiProps->mCrystalBottom.getAsset(), uiProps->mCrystalBottomNoteOffset, 61, 255, 440, uiProps, cont, BC_QUIT);
    }

    void MainMenu::_addCrystal(DragonGui &gui, odDb::Model *crystalModel, float noteOffset, float dia, float x, float y,
            UserInterfaceProperties *uiProps, odGui::Widget *cont, int buttonCode)
    {
        auto crystal = od::make_refd<CrystalRingButton>(gui, crystalModel,
                uiProps->mInnerRing.getAsset(),
                uiProps->mOuterRing.getAsset(),
                uiProps->mHoverSoundLooped.getAsset(),
                noteOffset);
        crystal->setDimensions(dia, dia, odGui::WidgetDimensionType::Pixels);
        crystal->setPosition(x/512, y/512);
        crystal->setOrigin(odGui::WidgetOrigin::Center);
        crystal->setClickedCallback(std::bind(&MainMenu::_buttonClicked, this, std::placeholders::_1), buttonCode);
        cont->addChild(crystal);
    }

    void MainMenu::_buttonClicked(int buttonCode)
    {
        switch(buttonCode)
        {
        default:
        case BC_MULTIPLAYER:
        case BC_LOAD:
        case BC_NEW:
        case BC_SAVE:
        case BC_OPTIONS:
        case BC_CREDITS:
            break;

        case BC_QUIT:
            Logger::info() << "Exit button clicked. Quitting game";
            getGui().quit();
            break;
        }
    }
}
