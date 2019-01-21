/*
 * HumanControl.cpp
 *
 *  Created on: 2 Apr 2018
 *      Author: zal
 */

#include <dragonRfl/classes/HumanControl.h>

#include <odCore/LevelObject.h>
#include <odCore/Level.h>
#include <odCore/Engine.h>

#include <odCore/rfl/Rfl.h>
#include <odCore/rfl/PrefetchProbe.h>

#include <odCore/input/InputManager.h>
#include <odCore/input/Keys.h>

#include <odCore/audio/SoundSystem.h>

#include <dragonRfl/RflDragon.h>

namespace dragonRfl
{

    HumanControl::HumanControl(DragonRfl &rfl)
    : mRfl(rfl)
    , mYaw(0)
	, mPitch(0)
    , mPrevYaw(0)
	, mForwardSpeed(0)
	, mRightSpeed(0)
    , mPlayerObject(nullptr)
    {
    }

    HumanControl::~HumanControl()
    {
    }

    void HumanControl::onLoaded(od::LevelObject &obj)
    {
        if(mRfl.getLocalPlayer() != nullptr)
        {
            Logger::warn() << "Duplicate HumanControl objects found in level. Destroying duplicate";
            obj.requestDestruction();
            return;
        }

        obj.setSpawnStrategy(od::SpawnStrategy::Always);

        mPlayerObject = &obj;
        mRfl.setLocalPlayer(this);

        // prefetch referenced assets
        odRfl::PrefetchProbe probe(mPlayerObject->getClass()->getAssetProvider());
        this->probeFields(probe);

        // configure controls
        odInput::InputManager &im = obj.getLevel().getEngine().getInputManager();
        auto actionHandler = std::bind(&HumanControl::_handleMovementAction, this, std::placeholders::_1, std::placeholders::_2);

        mForwardAction = im.getOrCreateAction(Action::Forward);
        mForwardAction->setRepeatable(false);
        mForwardAction->addCallback(actionHandler);
        mForwardAction->bindToKey(odInput::Key::W); // for testing only. we want to do this via the Drakan.cfg parser later

        mBackwardAction = im.getOrCreateAction(Action::Backward);
        mBackwardAction->setRepeatable(false);
        mBackwardAction->addCallback(actionHandler);
        mBackwardAction->bindToKey(odInput::Key::S); // for testing only. we want to do this via the Drakan.cfg parser later
    }

    void HumanControl::onSpawned(od::LevelObject &obj)
    {
    	Logger::verbose() << "Spawned Human Control at "
    			<< obj.getPosition().x << "/"
				<< obj.getPosition().y << "/"
				<< obj.getPosition().z;

    	glm::vec3 playerLookDirection = glm::eulerAngles(obj.getRotation());
    	mPitch = playerLookDirection.x;
    	mYaw = playerLookDirection.y;

    	obj.setEnableRflUpdateHook(true);
    }

    void HumanControl::onUpdate(od::LevelObject &obj, float relTime)
    {
    }

    void HumanControl::onMoved(od::LevelObject &obj)
    {
        odAudio::SoundSystem *soundSystem = obj.getLevel().getEngine().getSoundSystem();
        if(soundSystem != nullptr)
        {
            glm::vec3 pos = obj.getPosition();

            glm::quat lookDirection = obj.getRotation();
            glm::vec3 at = lookDirection * glm::vec3(0, 0, -1);
            glm::vec3 up = glm::vec3(0, 1, 0);

            soundSystem->setListenerPosition(pos);
            soundSystem->setListenerOrientation(at, up);
        }
    }

    void HumanControl::moveForward(float speed)
    {
    	mForwardSpeed = speed;
    }

	void HumanControl::moveRight(float speed)
    {
    	mRightSpeed = speed;
    }

	glm::vec3 HumanControl::getPosition()
    {
    	if(mPlayerObject == nullptr)
    	{
    		return glm::vec3(0,0,0);
    	}

    	return mPlayerObject->getPosition();
    }

	od::LevelObject &HumanControl::getLevelObject()
	{
	    return *mPlayerObject;
	}

    void HumanControl::_handleMovementAction(odInput::ActionHandle<Action> *action, odInput::InputEvent event)
    {
        switch(action->getAction())
        {
        case Action::Forward:
            Logger::info() << "I'm walking forward!";
            break;

        case Action::Backward:
            Logger::info() << "I'm walking backward!";
            break;

        default:
            break;
        }
    }

    OD_REGISTER_RFLCLASS(DragonRfl, HumanControl);

}
