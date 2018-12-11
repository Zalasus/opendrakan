/*
 * DragonGui.h
 *
 *  Created on: 11 Dec 2018
 *      Author: zal
 */

#ifndef INCLUDE_DRAGONRFL_GUI_DRAGONGUI_H_
#define INCLUDE_DRAGONRFL_GUI_DRAGONGUI_H_

#include <memory>

#include <odCore/db/AssetProvider.h>
#include <odCore/db/TextureFactory.h>
#include <odCore/db/Database.h>

namespace od
{
    class Engine;
}

namespace dragonRfl
{

    class UserInterfaceProperties;

    /**
     * Class handling Drakan's GUI, as well as providing GUI resources via Dragon.rrc and Interface.db.
     */
    class DragonGui : public odDb::AssetProvider
    {
    public:

        DragonGui(od::Engine &engine);
        ~DragonGui();

        inline UserInterfaceProperties *getUserInterfaceProperties() { return mUserInterfaceProperties.get(); }


    protected:

        virtual odDb::Texture *getTexture(od::RecordId recordId) override;


    private:

        od::SrscFile mRrcFile;
        odDb::TextureFactory mRrcTextureFactory;
        odDb::Database *mInterfaceDb;

        std::unique_ptr<UserInterfaceProperties> mUserInterfaceProperties;
    };

}

#endif /* INCLUDE_DRAGONRFL_GUI_DRAGONGUI_H_ */
