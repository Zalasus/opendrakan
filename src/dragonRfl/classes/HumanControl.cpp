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
    , mPlayerObject(nullptr)
    , mState(State::Idling)
    , mLastUpdatedYaw(0)
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

        mCursorListener = im.createCursorListener();
        mCursorListener->setCallback(std::bind(&HumanControl::_handleCursorMovement, this, std::placeholders::_1));
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

    	odRender::ObjectNode *objectNode = obj.getRenderNode();
        odAnim::Skeleton *skeleton = obj.getOrCreateSkeleton();
        if(skeleton != nullptr)
        {
            mAnimPlayer = std::make_unique<odAnim::SkeletonAnimationPlayer>(objectNode, skeleton);
            mAnimPlayer->setRootNodeAccumulator(this);

            mAnimPlayer->setRootNodeAccumulationModes(odAnim::AxesModes{ odAnim::AccumulationMode::Bone,
                                                                         odAnim::AccumulationMode::Bone,
                                                                         odAnim::AccumulationMode::Bone
                                                                       });

            mAnimPlayer->playAnimation(mReadyAnim.getAsset(), odAnim::PlaybackType::Looping, 1.0f);

        }else
        {
            Logger::warn() << "Used Human Control class on object without skeleton";
        }

    	obj.setEnableRflUpdateHook(true);
    }

    void HumanControl::onUpdate(od::LevelObject &obj, float relTime)
    {
        static const float turnAnimThreshold = M_PI/2; // angular yaw speed at which turn animation is triggered (in rad/sec)

        obj.setRotation(glm::quat(glm::vec3(0, mYaw, 0)));

        float yawSpeed = (mYaw - mLastUpdatedYaw)/relTime;
        mLastUpdatedYaw = mYaw;

        // handle state transitions that might happen during update
        switch(mState)
        {
        case State::Idling:
        case State::TurningLeft:
        case State::TurningRight:
            if(yawSpeed >= turnAnimThreshold)
            {
                _playAnim(mTurnLeft, true, false);
                mState = State::TurningLeft;

            }else if(yawSpeed <= -turnAnimThreshold)
            {
                _playAnim(mTurnRight, true, false);
                mState = State::TurningRight;

            }else if(mState != State::Idling)
            {
                if(mAnimPlayer != nullptr && mAnimPlayer->isPlaying())
                {
                    break; // wait till turn anim is done
                }

                _playAnim(mReadyAnim, true, false);
                mState = State::Idling;
            }
            break;

        default:
            break;
        }
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

	void HumanControl::moveRelative(const glm::vec3 &relTranslation, float relTime)
	{
	    if(mPlayerObject != nullptr)
	    {
	        mPlayerObject->setPosition(mPlayerObject->getPosition() + mPlayerObject->getRotation()*relTranslation);
	    }
	}

    void HumanControl::_handleMovementAction(odInput::ActionHandle<Action> *action, odInput::InputEvent event)
    {
        if(event.type == odInput::InputEvent::Type::Down)
        {
            switch(action->getAction())
            {
            case Action::Forward:
                _playAnim(mRunAnim, false, true);
                mState = State::RunningForward;
                break;

            case Action::Backward:
                _playAnim(mRunBackwards, false, true);
                mState = State::RunningBackward;
                break;

            default:
                break;
            }

        }else
        {
            _playAnim(mReadyAnim, true, true);
            mState = State::Idling;
        }
    }

    void HumanControl::_handleCursorMovement(const glm::vec2 &posNdc)
    {
        mPitch = ( M_PI/2) * posNdc.y;
        mYaw   = (-M_PI) * posNdc.x;
    }

    void HumanControl::_playAnim(const odRfl::AnimRef &animRef, bool skeletonOnly, bool looping)
    {
        static const odAnim::AxesModes walkAccum{  odAnim::AccumulationMode::Accumulate,
                                                   odAnim::AccumulationMode::Bone,
                                                   odAnim::AccumulationMode::Accumulate
                                                };

        static const odAnim::AxesModes fixedAccum{ odAnim::AccumulationMode::Bone,
                                                   odAnim::AccumulationMode::Bone,
                                                   odAnim::AccumulationMode::Bone
                                                 };

        auto playbackType = looping ? odAnim::PlaybackType::Looping : odAnim::PlaybackType::Normal;

        if(mAnimPlayer != nullptr)
        {
            mAnimPlayer->playAnimation(animRef.getAsset(), playbackType, 0.5f);
            mAnimPlayer->setRootNodeAccumulationModes(skeletonOnly ? fixedAccum : walkAccum);
        }
    }


    OD_REGISTER_RFLCLASS(DragonRfl, HumanControl);

}
