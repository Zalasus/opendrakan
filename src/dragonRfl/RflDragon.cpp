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

    void DragonRfl::spawnHumanControlForPlayer(od::Server &localServer, odNet::ClientId clientId)
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

            obj.stop();
            obj.despawn();
            auto newHumanControl = std::make_unique<HumanControl_Sv>(clientId);
            newHumanControl->setServer(localServer); // i kinda dislike having to do this manually
            obj.setRflClassInstance(std::move(newHumanControl));
            obj.spawn();
            obj.start();

            // TODO: make this a broadcast
            localServer.getMessageDispatcherForClient(clientId).sendGlobalMessage(MessageChannel::HUMANCONTROL_CREATED)
                << clientId << obj.getObjectId();
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

        _bindActions(im);

        auto &menuAction = im.getAction(Action::Main_Menu);
        menuAction.setRepeatable(false);
        menuAction.setIgnoreUpEvents(true);
        menuAction.addCallback([this](auto action, auto state)
        {
            mGui->setMenuMode(!mGui->isMenuMode());
        });

        auto &physicsDebugAction = im.getAction(Action::PhysicsDebug_Toggle);
        physicsDebugAction.setRepeatable(false);
        physicsDebugAction.setIgnoreUpEvents(true);
        physicsDebugAction.addCallback([&localClient](Action action, odInput::ActionState state)
        {
            localClient.getPhysicsSystem().toggleDebugDrawing();
        });


        // TODO: the existence of this should be coupled to the existence of the localClient somehow. maybe we *should* split local behaviour off into client/server RFLs
        mControlCreationChannelListener = localClient.getMessageDispatcher().createGlobalListener(MessageChannel::HUMANCONTROL_CREATED);
        mControlCreationChannelListener->setCallback([this, &localClient](auto channel, auto messageBodyReader) mutable
        {
            odNet::ClientId clientId;
            od::LevelObjectId objectId;
            messageBodyReader >> clientId >> objectId;

            /*if(clientId != localClient.getClientId())
            {
                // this is not the controller for this client. later, we might store this association somewhere, or create
                //  a custom HumanControlDummy_Cl that displays player colors, team, etc. for now, just ignore
                return;

            }else*/
            {
                auto controlObject = localClient.getLevel()->getLevelObjectById(objectId);
                if(controlObject == nullptr)
                {
                    Logger::warn() << "Got HUMANCONTROL_CREATED message with invalid object ID " << objectId;

                }else
                {
                    controlObject->stop();
                    controlObject->despawn();
                    auto newControl = std::make_unique<HumanControl_Cl>();
                    newControl->setClient(localClient); // i kinda dislike having to do this manually
                    controlObject->setRflClassInstance(std::move(newControl));
                    controlObject->spawn();
                    controlObject->start();
                }
            }
        });
    }

    void DragonRfl::onLevelLoaded(od::Server &localServer)
    {
        localServer.forEachClient([this, &localServer](odNet::ClientId id)
        {
            this->spawnHumanControlForPlayer(localServer, id);
        });
    }

    void DragonRfl::_bindActions(odInput::InputManager &im)
    {
        // these actions stay hardcoded:
        im.bindAction(Action::Main_Menu,           odInput::Key::Escape);
        im.bindAction(Action::PhysicsDebug_Toggle, odInput::Key::F3);

        // later, these are to be read from the Drakan.cfg. For now, we hardcode them here for testing reasons
        im.bindAction(Action::Forward,          odInput::Key::W);
        im.bindAction(Action::Backward,         odInput::Key::S);
        im.bindAction(Action::Attack_Primary,   odInput::Key::Mouse_Left);
        im.bindAction(Action::Attack_Secondary, odInput::Key::Mouse_Middle);
        im.bindAction(Action::Attack_Secondary, odInput::Key::KP_1);

        im.bindAnalogAction(Action::Look, odInput::AnalogSource::MOUSE_POSITION);
    }

    void DragonRfl::_handleAction(Action action, odInput::ActionState state)
    {
        switch(action)
        {
        case Action::Main_Menu:

            break;

        case Action::PhysicsDebug_Toggle:
            break;

        default:
            break;
        }
    }

}
