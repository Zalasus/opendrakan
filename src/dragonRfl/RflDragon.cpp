/*
 * RflDragon.cpp
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#include <dragonRfl/RflDragon.h>

#include <odCore/Client.h>
#include <odCore/Server.h>
#include <odCore/Level.h>
#include <odCore/LevelObject.h>

#include <odCore/audio/SoundSystem.h>

#include <odCore/input/InputManager.h>

#include <odCore/rfl/PrefetchProbe.h>

#include <odCore/db/DbManager.h>

#include <odCore/render/Renderer.h>

#include <odCore/physics/PhysicsSystem.h>

#include <dragonRfl/Actions.h>
#include <dragonRfl/gui/DragonGui.h>
#include <dragonRfl/classes/HumanControl.h>

namespace dragonRfl
{

    DragonRfl::DragonRfl()
    : mLocalPlayer(nullptr)
    {
    }

    DragonRfl::~DragonRfl()
    {
    }

    void DragonRfl::spawnHumanControlForPlayer(od::Server &localServer, odNet::ClientId client)
    {
        if(localServer.getLevel() == nullptr)
        {
            throw od::Exception("Tried to spawn human control on server without level");
        }

        // TODO: spawning behaviour is different between SP and MP. do the switch here!
        // in case of SP: locate HumanControl-dummy in level and replace it with a HumanControl_Sv instance

        std::vector<od::LevelObject*> foundObjects;
        localServer.getLevel()->findObjectsOfType(HumanControl::classId(), foundObjects);
        if(foundObjects.empty())
        {
            // TODO: intro level excepted
            Logger::error() << "Found no Human Control in level! This could be an error in level design.";

        }else
        {
            if(foundObjects.size() > 1)
            {
                Logger::warn() << "More than one Human Control found in level! Ignoring all but one";
            }

            auto &obj = *foundObjects.back();

             // i kinda dislike that we need to set everything up ourselves
            auto newHumanControl = std::make_unique<HumanControl_Sv>(client);
            newHumanControl->setServer(localServer);
            obj.getClass()->fillFields(newHumanControl->getFields());
            newHumanControl->onLoaded();

            obj.replaceRflClassInstance(std::move(newHumanControl));
        }
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
        im.bindActionToKey(menuAction, odInput::Key::Escape);
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

    void DragonRfl::onLevelLoaded(od::Server &localServer)
    {
        localServer.forEachClient([this, &localServer](odNet::ClientId id)
        {
            this->spawnHumanControlForPlayer(localServer, id);
        });
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
