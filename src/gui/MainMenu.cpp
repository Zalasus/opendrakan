/*
 * MainMenu.cpp
 *
 *  Created on: 29 Jun 2018
 *      Author: zal
 */

#include "gui/MainMenu.h"

#include "gui/GuiManager.h"
#include "gui/TexturedQuad.h"
#include "gui/GuiTextures.h"

namespace od
{


    class MainMenuImage : public DrawableWidget
    {
    public:

        MainMenuImage(GuiManager &gm)
        : DrawableWidget(gm)
        {
            osg::ref_ptr<TexturedQuad> topLeft = new TexturedQuad;
            topLeft->setTextureImage(gm.getTexture(GuiTextures::MainMenu_TopLeft));
            topLeft->setTextureCoordsFromPixels(osg::Vec2(0, 0), osg::Vec2(255, 255));
            topLeft->setVertexCoords(osg::Vec2(0.0, 0.0), osg::Vec2(0.5, 0.5));
            this->addDrawable(topLeft);

            osg::ref_ptr<TexturedQuad> topRight = new TexturedQuad;
            topRight->setTextureImage(gm.getTexture(GuiTextures::MainMenu_TopRight));
            topRight->setTextureCoordsFromPixels(osg::Vec2(0, 0), osg::Vec2(255, 255));
            topRight->setVertexCoords(osg::Vec2(0.5, 0.0), osg::Vec2(1, 0.5));
            this->addDrawable(topRight);

            osg::ref_ptr<TexturedQuad> bottomLeft = new TexturedQuad;
            bottomLeft->setTextureImage(gm.getTexture(GuiTextures::MainMenu_BottomLeft));
            bottomLeft->setTextureCoordsFromPixels(osg::Vec2(0, 0), osg::Vec2(255, 255));
            bottomLeft->setVertexCoords(osg::Vec2(0.0, 0.5), osg::Vec2(0.5, 1));
            this->addDrawable(bottomLeft);

            osg::ref_ptr<TexturedQuad> bottomRight = new TexturedQuad;
            bottomRight->setTextureImage(gm.getTexture(GuiTextures::MainMenu_BottomRight));
            bottomRight->setTextureCoordsFromPixels(osg::Vec2(0, 0), osg::Vec2(255, 255));
            bottomRight->setVertexCoords(osg::Vec2(0.5, 0.5), osg::Vec2(1.0, 1.0));
            this->addDrawable(bottomRight);

            this->setDimensions(512.0, 512.0, WidgetDimensionType::Pixels);
        }

    };

    class MainMenuBackground : public DrawableWidget
    {
    public:

        MainMenuBackground(GuiManager &gm)
        : DrawableWidget(gm)
        {
            osg::ref_ptr<TexturedQuad> bg = new TexturedQuad;
            bg->setVertexCoords(osg::Vec2(0.0, 0.0), osg::Vec2(1.0, 1.0));
            bg->setColor(osg::Vec4(0.0, 0.0, 0.0, 0.5));
            this->addDrawable(bg);

            this->setDimensions(1.0, 1.0, WidgetDimensionType::ParentRelative);
        }
    };



    MainMenu::MainMenu(GuiManager &gm, odRfl::UserInterfaceProperties *uiProps)
    : ContainerWidget(gm)
    {
        osg::ref_ptr<MainMenuImage> imageWidget(new MainMenuImage(gm));
        imageWidget->setZIndex(0);
        imageWidget->setOrigin(WidgetOrigin::Center);
        imageWidget->setPosition(osg::Vec2(0.5, 0.5));
        this->addWidget(imageWidget);

        osg::ref_ptr<MainMenuBackground> bgWidget(new MainMenuBackground(gm));
        bgWidget->setZIndex(1);
        this->addWidget(bgWidget);

        //osg::ref_ptr<Model> c = uiProps->mCrystalMiddle.getAsset();
        //imageWidget->addChild(c);

        this->setDimensions(1.0, 1.0, WidgetDimensionType::ParentRelative);
    }


}
