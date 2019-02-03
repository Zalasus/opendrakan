/*
 * RflDragon.h
 *
 *  Created on: 16 Sep 2018
 *      Author: zal
 */

#ifndef INCLUDE_DRAGONRFL_RFLDRAGON_H_
#define INCLUDE_DRAGONRFL_RFLDRAGON_H_

#include <memory>

#include <odCore/input/Action.h>

#include <odCore/rfl/Rfl.h>

#include <dragonRfl/Actions.h>

namespace dragonRfl
{

    class LocalPlayer;
    class DragonGui;

    class DragonRfl : public odRfl::AutoRegisteringRfl<DragonRfl>
    {
    public:

        DragonRfl(od::Engine &engine);
        ~DragonRfl();

        inline LocalPlayer *getLocalPlayer() { return mLocalPlayer; }
        inline void setLocalPlayer(LocalPlayer *lp) { mLocalPlayer = lp; }

        virtual void onStartup() override;


    private:

        void _handleAction(odInput::ActionHandle<Action> *action, odInput::InputEvent event);

        LocalPlayer *mLocalPlayer;
        std::unique_ptr<DragonGui> mGui;

        od::RefPtr<odInput::IAction> mMenuAction;
    };

}

OD_DEFINE_RFL_TRAITS("Dragon", dragonRfl::DragonRfl);

#endif /* INCLUDE_DRAGONRFL_RFLDRAGON_H_ */
