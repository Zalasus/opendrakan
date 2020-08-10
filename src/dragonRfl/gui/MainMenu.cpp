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

#include <odCore/gui/Quad.h>

namespace dragonRfl
{

    class MainMenuImage : public odGui::Widget
    {
    public:

        MainMenuImage(DragonGui &gui)
        : odGui::Widget(gui)
        {
            odGui::Quad topLeft(gui.getRenderer());
            topLeft.setTextureFromDb(gui, GuiTextures::MainMenu_TopLeft, gui.getRenderer());
            topLeft.setTextureCoordsFromPixels(glm::vec2(0, 0), glm::vec2(255, 255));
            topLeft.setVertexCoords(glm::vec2(0.0, 0.0), glm::vec2(0.5, 0.5));
            this->addRenderHandle(topLeft.getHandle());

            odGui::Quad topRight(gui.getRenderer());
            topRight.setTextureFromDb(gui, GuiTextures::MainMenu_TopRight, gui.getRenderer());
            topRight.setTextureCoordsFromPixels(glm::vec2(0, 0), glm::vec2(255, 255));
            topRight.setVertexCoords(glm::vec2(0.5, 0.0), glm::vec2(1, 0.5));
            this->addRenderHandle(topRight.getHandle());

            odGui::Quad bottomLeft(gui.getRenderer());
            bottomLeft.setTextureFromDb(gui, GuiTextures::MainMenu_BottomLeft, gui.getRenderer());
            bottomLeft.setTextureCoordsFromPixels(glm::vec2(0, 0), glm::vec2(255, 255));
            bottomLeft.setVertexCoords(glm::vec2(0.0, 0.5), glm::vec2(0.5, 1));
            this->addRenderHandle(bottomLeft.getHandle());

            odGui::Quad bottomRight(gui.getRenderer());
            bottomRight.setTextureFromDb(gui, GuiTextures::MainMenu_BottomRight, gui.getRenderer());
            bottomRight.setTextureCoordsFromPixels(glm::vec2(0, 0), glm::vec2(255, 255));
            bottomRight.setVertexCoords(glm::vec2(0.5, 0.5), glm::vec2(1.0, 1.0));
            this->addRenderHandle(bottomRight.getHandle());

            this->setDimensions({512.0, 512.0}, odGui::WidgetDimensionType::Pixels);
        }

    };

    class MainMenuBackground : public odGui::Widget
    {
    public:

        MainMenuBackground(DragonGui &gui)
        : odGui::Widget(gui)
        {
            odGui::Quad bg(gui.getRenderer());
            bg.setVertexCoords(glm::vec2(0.0, 0.0), glm::vec2(1.0, 1.0));
            bg.setColor(glm::vec4(0.0, 0.0, 0.0, 0.5));
            this->addRenderHandle(bg.getHandle());

            this->setDimensions({1.0, 1.0}, odGui::WidgetDimensionType::ParentRelative);
        }
    };



    MainMenu::MainMenu(DragonGui &gui, UserInterfacePropertiesFields *uiProps)
    : odGui::Widget(gui)
    , mDragonGui(gui)
    {
        auto imageWidget = std::make_shared<MainMenuImage>(gui);
        imageWidget->setOrigin(odGui::WidgetOrigin::Center);
        imageWidget->setPosition(glm::vec2(0.5, 0.5));
        imageWidget->setZPosition(0.5);
        this->addChild(imageWidget);

        auto bgWidget = std::make_shared<MainMenuBackground>(gui);
        bgWidget->setZPosition(1.0);
        this->addChild(bgWidget);

        this->setDimensions({1.0, 1.0}, odGui::WidgetDimensionType::ParentRelative);

        auto cont = std::make_shared<Widget>(gui);
        cont->setDimensions({512, 512}, odGui::WidgetDimensionType::Pixels);
        cont->setOrigin(odGui::WidgetOrigin::Center);
        cont->setPosition({0.5, 0.5});
        this->addChild(cont);

        _addCrystal(gui, uiProps->crystalTop.getAsset(), uiProps->crystalTopNoteOffset, 53, 255, 62, uiProps, cont, BC_MULTIPLAYER);
        _addCrystal(gui, uiProps->crystalLeft.getAsset(), uiProps->crystalLeftNoteOffset, 57, 110, 195, uiProps, cont, BC_LOAD);
        _addCrystal(gui, uiProps->crystalMiddle.getAsset(), uiProps->crystalMiddleNoteOffset, 67, 255, 191, uiProps, cont, BC_NEW);
        _addCrystal(gui, uiProps->crystalRight.getAsset(), uiProps->crystalRightNoteOffset, 57, 400, 195, uiProps, cont, BC_SAVE);
        _addCrystal(gui, uiProps->crystalLowerLeft.getAsset(), uiProps->crystalLowerLeftNoteOffset, 35, 152, 292, uiProps, cont, BC_OPTIONS);
        _addCrystal(gui, uiProps->crystalLowerRight.getAsset(), uiProps->crystalLowerRightNoteOffset, 35, 358, 292, uiProps, cont, BC_CREDITS);
        _addCrystal(gui, uiProps->crystalBottom.getAsset(), uiProps->crystalBottomNoteOffset, 61, 255, 440, uiProps, cont, BC_QUIT);
    }

    void MainMenu::_addCrystal(DragonGui &gui, std::shared_ptr<odDb::Model> crystalModel, float noteOffset, float dia, float x, float y,
            UserInterfacePropertiesFields *uiProps, std::shared_ptr<odGui::Widget> cont, int buttonCode)
    {
        auto crystal = std::make_shared<CrystalRingButton>(gui, crystalModel,
                uiProps->innerRing.getAsset(),
                uiProps->outerRing.getAsset(),
                uiProps->hoverSoundLooped.getAsset(),
                noteOffset);
        crystal->setDimensions({dia, dia}, odGui::WidgetDimensionType::Pixels);
        crystal->setPosition({x/512, y/512});
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
