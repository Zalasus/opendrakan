/*
 * DragonGui.h
 *
 *  Created on: 11 Dec 2018
 *      Author: zal
 */

#ifndef INCLUDE_DRAGONRFL_GUI_DRAGONGUI_H_
#define INCLUDE_DRAGONRFL_GUI_DRAGONGUI_H_

#include <memory>
#include <optional>
#include <unordered_map>

#include <odCore/FilePath.h>

#include <odCore/db/TextureFactory.h>
#include <odCore/db/Database.h>

#include <odCore/gui/Gui.h>
#include <odCore/gui/Quad.h>

#include <dragonRfl/classes/UserInterfaceProperties.h>

namespace od
{
    class Client;
}

namespace odDb
{
    class DependencyTable;
}

namespace dragonRfl
{

    class MainMenu;

    /**
     * Class handling Drakan's GUI, as well as providing GUI resources via Dragon.rrc and Interface.db.
     */
    class DragonGui final : public odGui::Gui
    {
    public:

        DragonGui(od::Client &client);
        ~DragonGui();

        inline od::Client &getClient() { return mClient; }
        inline const UserInterfacePropertiesFields &getUserInterfaceProperties() const { return mUserInterfaceProperties; }
        inline odDb::TextureFactory &getGuiTextureFactory() { return mRrcTextureFactory; }

        /**
         * @brief Localizes string with localization tag.
         *
         * Will check whether passed string begins with a localization tag "<0x...>". If not, the string is returned as-is.
         * If it does, the string ID in the tag will be looked up and the localized version of that string returned.
         * If the string ID can not be found in the RRC, the tag will be stripped and the unlocalized string returned.
         */
        std::string localizeString(const std::string &s);

        /**
         * @brief Returns the string with the given ID or an empty option if not found.
         */
        std::optional<std::string> getStringById(od::RecordId stringId);

        odGui::Quad makeQuadFromGuiTexture(od::RecordId id);

        virtual void onMenuModeChanged() override;


    private:

        void _decryptString(char * const str, const size_t len);

        od::Client &mClient;
        od::SrscFile mRrcFile;
        std::shared_ptr<odDb::DependencyTable> mDummyDependencyTable;
        odDb::TextureFactory mRrcTextureFactory;
        std::shared_ptr<odDb::Database> mInterfaceDb;

        UserInterfacePropertiesFields mUserInterfaceProperties;

        std::unordered_map<od::RecordId, std::string> mLocalizedStringCache;

        std::shared_ptr<MainMenu> mMainMenu;
    };

}

#endif /* INCLUDE_DRAGONRFL_GUI_DRAGONGUI_H_ */
