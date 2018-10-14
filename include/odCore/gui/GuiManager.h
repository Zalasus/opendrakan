/*
 * GuiManager.h
 *
 *  Created on: 8 Jun 2018
 *      Author: zal
 */

#ifndef INCLUDE_GUI_GUIMANAGER_H_
#define INCLUDE_GUI_GUIMANAGER_H_

#include <string>

#include <map>
#include <osgViewer/Viewer>

#include <odCore/SrscFile.h>
#include <odCore/db/TextureFactory.h>
#include <odCore/db/AssetProvider.h>
#include <odCore/db/Database.h>
#include <odCore/gui/WidgetIntersectVisitor.h>

namespace od
{
    class Engine;
}

namespace odGui
{

    class Widget;

    /**
     * Class for managing the game's GUI, as well as an interface for accessing the
     * RRC file which contains GUI textures and localized GUI text.
     */
    class GuiManager : public odDb::AssetProvider
    {
    public:

        GuiManager(od::Engine &engine, osgViewer::Viewer *viewer);

        inline od::Engine &getEngine() { return mEngine; }
        inline bool isMenuMode() const { return mMenuMode; }

        void quit();

        osg::Vec2 getScreenResolution();

        void addWidget(Widget *widget);
        void removeWidget(Widget *widget);
        size_t getWidgetCount();
        std::pair<int32_t, int32_t> getWidgetZRange();

        void setMenuMode(bool b);

        void mouseDown();

        /**
         * @brief Sets the widget to be used as a cursor.
         *
         * This is purely visual. The mouse picking functionality works without an assigned cursor widget.
         */
        void setCursorWidget(Widget *cursor);

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
        std::string getStringById(od::RecordId stringId);

        void dumpStrings();
        void dumpTextures();


    protected:

        // override AssetProvider
        virtual odDb::Texture *getTexture(od::RecordId recordId) override;


    private:

        inline void _decryptString(char * const str, const size_t len);
        void _setupGui();

        od::Engine &mEngine;
        osg::ref_ptr<osgViewer::Viewer> mViewer;
        od::SrscFile mRrcFile;
        odDb::TextureFactory mTextureFactory;
        bool mMenuMode;

        std::map<od::RecordId, std::string> mLocalizedStringCache;

        osg::ref_ptr<osg::Camera> mGuiCamera;
        osg::ref_ptr<osg::Group>  mGuiRoot;
        std::deque<osg::ref_ptr<Widget>> mWidgets;
        osg::Vec2 mCursorPosInNdc;
        osg::ref_ptr<Widget> mCursorWidget;

        osg::Matrix mWidgetToScreenSpaceXform; // FIXME: these are badly named. this goes from widget space to NDC, not screen space
        osg::Matrix mScreenToWidgetSpaceXform;
        osg::ref_ptr<WidgetIntersectVisitor> mWidgetIntersectVisitor;
        std::vector<WidgetHitInfo> mLastHitWidgets;
        std::vector<WidgetHitInfo> mJoinedHitWidgets; // vector for combining last and current hits
    };

}

#endif /* INCLUDE_GUI_GUIMANAGER_H_ */
