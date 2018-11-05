/*
 * RflDragon.h
 *
 *  Created on: 16 Sep 2018
 *      Author: zal
 */

#ifndef INCLUDE_DRAGONRFL_RFLDRAGON_H_
#define INCLUDE_DRAGONRFL_RFLDRAGON_H_

#include <odCore/rfl/Rfl.h>
#include <odCore/db/Database.h>

namespace dragonRfl
{

    class UserInterfaceProperties;

    class DragonRfl : public odRfl::AutoRegisteringRfl<DragonRfl>
    {
    public:

        DragonRfl(od::Engine &engine);

        virtual void onStartup() override;
        virtual void onMenuToggle(bool newMode) override;


    private:

        odDb::Database *mInterfaceDb;
        std::unique_ptr<UserInterfaceProperties> mUserInterfacePropertiesInstance;

    };

}

OD_DEFINE_RFL_TRAITS("Dragon", dragonRfl::DragonRfl);

#endif /* INCLUDE_DRAGONRFL_RFLDRAGON_H_ */
