/*
 * DragonGui.h
 *
 *  Created on: 11 Dec 2018
 *      Author: zal
 */

#ifndef INCLUDE_DRAGONRFL_GUI_DRAGONGUI_H_
#define INCLUDE_DRAGONRFL_GUI_DRAGONGUI_H_

#include <memory>

#include <odCore/FilePath.h>

#include <odCore/db/AssetProvider.h>
#include <odCore/db/TextureFactory.h>
#include <odCore/db/Database.h>

#include <odCore/gui/Gui.h>

namespace od
{
    class Client;
}

namespace dragonRfl
{

    class UserInterfaceProperties;
    class MainMenu;

    /**
     * Class handling Drakan's GUI, as well as providing GUI resources via Dragon.rrc and Interface.db.
     */
    class DragonGui : public odGui::Gui, public odDb::AssetProvider
    {
    public:

        DragonGui(od::Client &client);
        ~DragonGui();

        inline od::Client &getClient() { return mClient; }
        inline UserInterfaceProperties *getUserInterfaceProperties() { return mUserInterfaceProperties.get(); }

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

        virtual void onMenuModeChanged() override;


    protected:

        virtual od::RefPtr<odDb::Texture> getTexture(od::RecordId recordId) override;


    private:

        void _decryptString(char * const str, const size_t len);

        od::Client &mClient;
        od::SrscFile mRrcFile;
        odDb::TextureFactory mRrcTextureFactory;
        odDb::Database *mInterfaceDb;

        std::unique_ptr<UserInterfaceProperties> mUserInterfaceProperties;

        std::map<od::RecordId, std::string> mLocalizedStringCache;

        od::RefPtr<MainMenu> mMainMenu;
    };

}

#endif /* INCLUDE_DRAGONRFL_GUI_DRAGONGUI_H_ */
