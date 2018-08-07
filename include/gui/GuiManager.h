/*
 * GuiManager.h
 *
 *  Created on: 8 Jun 2018
 *      Author: zal
 */

#ifndef INCLUDE_GUI_GUIMANAGER_H_
#define INCLUDE_GUI_GUIMANAGER_H_

#include <string>

#include <osgViewer/Viewer>

#include "FilePath.h"
#include "SrscFile.h"
#include "db/TextureFactory.h"
#include "db/AssetProvider.h"
#include "db/Database.h"
#include "gui/Cursor.h"
#include "gui/MainMenu.h"
#include "rfl/dragon/UserInterfaceProperties.h"

namespace od
{

    class Engine;

    class Widget;

    /**
     * Class for managing the game's GUI, as well as an interface for accessing the
     * RRC file which contains GUI textures and localized GUI text.
     */
    class GuiManager : public AssetProvider
    {
    public:

        GuiManager(Engine &engine, osgViewer::Viewer *viewer);

        inline Engine &getEngine() { return mEngine; }
        inline bool isMenuMode() const { return mMenuMode; }

        void quit();

        osg::Vec2 getScreenResolution();

        void addWidget(Widget *widget);
        size_t getWidgetCount();
        std::pair<int32_t, int32_t> getWidgetZRange();

        void setMenuMode(bool b);
        void setShowMainMenu(bool b);

        /**
         * @brief Positions cursor in screen space.
         */
        void setCursorPosition(const osg::Vec2 &pos);

        /**
         * @brief Localizes string with localization tag.
         *
         * Will check whether passed string begins with a localization tag "<0x...>". If not, the string is returned as-is.
         * If it does, the string ID in the tag will be looked up and the localized version of that string returned.
         * If the string ID can not be found in the RRC, the tag will be stripped and the unlocalized string returned.
         */
        std::string localizeString(const std::string &s);

        /**
         * @brief Returns the string with the given ID or throws if string can not be found.
         */
        std::string getStringById(RecordId stringId);

        void dumpStrings();
        void dumpTextures();

        // implement AssetProvider
        virtual Texture *getTextureByRef(const AssetRef &ref) override;
        virtual Texture *getTexture(RecordId recordId) override;


    private:

        // make all methods for non-textures inaccessible. can't do the opposite because the AssetProvider base must stay accessible
        using AssetProvider::getClassByRef;
        using AssetProvider::getModelByRef;
        using AssetProvider::getSequenceByRef;
        using AssetProvider::getAnimationByRef;
        using AssetProvider::getSoundByRef;
        using AssetProvider::getClass;
        using AssetProvider::getModel;
        using AssetProvider::getSequence;
        using AssetProvider::getAnimation;
        using AssetProvider::getSound;

        inline void _decryptString(char * const str, const size_t len);
        void _setupGui();

        Engine &mEngine;
        osg::ref_ptr<osgViewer::Viewer> mViewer;
        SrscFile mRrcFile;
        TextureFactory mTextureFactory;
        Database &mInterfaceDb;
        bool mMenuMode;

        osg::ref_ptr<osg::Camera> mGuiCamera;
        osg::ref_ptr<osg::Group>  mGuiRoot;
        std::deque<osg::ref_ptr<Widget>> mWidgets;
        std::unique_ptr<odRfl::UserInterfaceProperties> mUserInterfacePropertiesInstance;
        osg::ref_ptr<Cursor> mCursorWidget;
        osg::ref_ptr<MainMenu> mMainMenuWidget;

        osg::Matrix mWidgetToScreenSpaceXform;
        osg::Matrix mScreenToWidgetSpaceXform;
    };

}

#endif /* INCLUDE_GUI_GUIMANAGER_H_ */
