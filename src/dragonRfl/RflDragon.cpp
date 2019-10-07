/*
 * RflDragon.cpp
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#include <dragonRfl/RflDragon.h>

#include <odCore/Engine.h>
#include <odCore/Client.h>
#include <odCore/Server.h>

#include <odCore/audio/SoundSystem.h>

#include <odCore/input/InputManager.h>

#include <odCore/rfl/PrefetchProbe.h>

#include <odCore/db/DbManager.h>

#include <odCore/render/Renderer.h>

#include <odCore/physics/PhysicsSystem.h>

#include <dragonRfl/Actions.h>

#include <dragonRfl/gui/DragonGui.h>

#include <dragonRfl/classes/UserInterfaceProperties.h>

namespace dragonRfl
{

    DragonRfl::DragonRfl()
    : mLocalPlayer(nullptr)
    {
    }

    DragonRfl::~DragonRfl()
    {
    }

    const char *DragonRfl::getName() const
    {
        return "dragon";
    }

    void DragonRfl::onLoaded()
    {
        _registerClasses();
    }

    void DragonRfl::onGameStartup(od::Server &localServer, od::Client &localClient)
    {
        mGui = std::make_unique<DragonGui>(localClient);
        localClient.getInputManagerSafe().setGui(mGui.get());

        if(localClient.getSoundSystem() != nullptr)
        {
            od::FilePath musicRrc("Music.rrc", localClient.getEngineRootDir());
            localClient.getSoundSystem()->loadMusicContainer(musicRrc.adjustCase());
            localClient.getSoundSystem()->playMusic(1);
        }

        if(!localServer.hasInitialLevelOverride())
        {
            od::FilePath initialLevel(mGui->getUserInterfaceProperties()->mIntroLevelFilename);
            localServer.loadLevel(initialLevel.adjustCase());
        }

        odInput::InputManager *im = localClient.getInputManager();
        if(im != nullptr)
        {
            auto actionHandler = std::bind(&DragonRfl::_handleAction, this, std::placeholders::_1, std::placeholders::_2);
            auto mmAction = im->getOrCreateAction(Action::Main_Menu);
            mmAction->setRepeatable(false);
            mmAction->setIgnoreUpEvents(true);
            mmAction->addCallback(actionHandler);
            mmAction->bindToKey(odInput::Key::Return);
            mMenuAction = mmAction;

            auto physicsDebugAction = im->getOrCreateAction(Action::PhysicsDebug_Toggle);
            physicsDebugAction->setRepeatable(false);
            physicsDebugAction->setIgnoreUpEvents(true);
            physicsDebugAction->bindToKey(odInput::Key::F3);
            auto physDebugCallback = [&localClient](odInput::ActionHandle<Action> *action, odInput::InputEvent event)
            {
                localClient.getPhysicsSystem().toggleDebugDrawing();
            };
            physicsDebugAction->addCallback(physDebugCallback);
            mPhysicsDebugAction = physicsDebugAction;
        }
    }

    void DragonRfl::_handleAction(odInput::ActionHandle<Action> *action, odInput::InputEvent event)
    {
        switch(action->getAction())
        {
        case Action::Main_Menu:
            mGui->setMenuMode(!mGui->isMenuMode());
            break;

        case Action::PhysicsDebug_Toggle:
            break;

        default:
            break;
        }
    }

}
