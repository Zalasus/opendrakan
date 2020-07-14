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

    class DragonRfl : public odRfl::Rfl
    {
    public:

        DragonRfl();
        virtual ~DragonRfl();

        inline LocalPlayer *getLocalPlayer() { return mLocalPlayer; }
        inline void setLocalPlayer(LocalPlayer *lp) { mLocalPlayer = lp; }

        virtual const char *getName() const override;
        virtual void onLoaded() override;
        virtual void onGameStartup(od::Server &localServer, od::Client &localClient, bool loadIntroLevel) override;


    private:

        void _handleAction(odInput::ActionHandle<Action> *action, odInput::InputEvent event);

        // note: this is defined in ClassRegistry.cpp, as to avoid having to include all the class headers in the main RFL logic code
        void _registerClasses();

        LocalPlayer *mLocalPlayer;
        std::unique_ptr<DragonGui> mGui;

        od::RefPtr<odInput::IAction> mMenuAction;
        od::RefPtr<odInput::IAction> mPhysicsDebugAction;
    };

}

#endif /* INCLUDE_DRAGONRFL_RFLDRAGON_H_ */
