/*
 * RflDragon.cpp
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#include <dragonRfl/RflDragon.h>

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

    void DragonRfl::onGameStartup(od::Server &localServer, od::Client &localClient, bool loadIntroLevel)
    {
        mGui = std::make_unique<DragonGui>(localClient);
        localClient.getInputManager().setGui(mGui.get());

        if(localClient.getSoundSystem() != nullptr)
        {
            od::FilePath musicRrc("Music.rrc", localClient.getEngineRootDir());
            localClient.getSoundSystem()->loadMusicContainer(musicRrc.adjustCase());
            localClient.getSoundSystem()->playMusic(1);
        }

        if(loadIntroLevel)
        {
            od::FilePath levelPath(mGui->getUserInterfaceProperties().introLevelFilename, localServer.getEngineRootDir());
            localServer.loadLevel(levelPath.adjustCase());
        }

        odInput::InputManager &im = localClient.getInputManager();

        auto actionHandler = std::bind(&DragonRfl::_handleAction, this, std::placeholders::_1, std::placeholders::_2);
        auto menuAction = im.getOrCreateAction(Action::Main_Menu);
        menuAction->setRepeatable(false);
        menuAction->setIgnoreUpEvents(true);
        menuAction->addCallback(actionHandler);
        im.bindActionToKey(menuAction, odInput::Key::Return);
        mMenuAction = menuAction;

        auto physicsDebugAction = im.getOrCreateAction(Action::PhysicsDebug_Toggle);
        physicsDebugAction->setRepeatable(false);
        physicsDebugAction->setIgnoreUpEvents(true);
        auto physDebugCallback = [&localClient](Action action, odInput::ActionState state)
        {
            localClient.getPhysicsSystem().toggleDebugDrawing();
        };
        physicsDebugAction->addCallback(physDebugCallback);
        im.bindActionToKey(physicsDebugAction, odInput::Key::F3);
        mPhysicsDebugAction = physicsDebugAction;
    }

    void DragonRfl::_handleAction(Action action, odInput::ActionState state)
    {
        switch(action)
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
