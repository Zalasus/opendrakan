/*
 * RflDragon.h
 *
 *  Created on: 16 Sep 2018
 *      Author: zal
 */

#ifndef INCLUDE_DRAGONRFL_RFLDRAGON_H_
#define INCLUDE_DRAGONRFL_RFLDRAGON_H_

#include <memory>

#include <odCore/input/InputManager.h>

#include <odCore/net/IdTypes.h>
#include <odCore/net/MessageDispatcher.h>

#include <odCore/rfl/Rfl.h>

#include <dragonRfl/Actions.h>
#include <dragonRfl/MessageChannel.h>

namespace dragonRfl
{
    class DragonGui;

    class DragonRfl : public odRfl::Rfl
    {
    public:

        DragonRfl();
        virtual ~DragonRfl();

        /**
         * @brief Spawns a controller for the given client.
         */
        void spawnHumanControlForPlayer(od::Server &localServer, odNet::ClientId client);

        virtual const char *getName() const override;
        virtual void onLoaded() override;
        virtual void onGameStartup(od::Server &localServer, od::Client &localClient, bool loadIntroLevel) override;
        virtual void onLevelLoaded(od::Server &localServer) override;

        void setShowMenu(bool b);


    private:

        void _bindActions(odInput::InputManager &im);
        void _handleAction(Action action, odInput::ActionState state);

        // note: this is defined in ClassRegistry.cpp, as to avoid having to include all the class headers in the main RFL logic code
        void _registerClasses();

        std::unique_ptr<DragonGui> mGui;

        std::shared_ptr<odNet::GlobalMessageListener<MessageChannel>> mControlCreationChannelListener;
    };

}

#endif /* INCLUDE_DRAGONRFL_RFLDRAGON_H_ */
