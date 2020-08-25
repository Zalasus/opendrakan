/*
 * HumanControl.cpp
 *
 *  Created on: 2 Apr 2018
 *      Author: zal
 */

#include <dragonRfl/classes/HumanControl.h>

#include <glm/gtc/constants.hpp>

#include <dragonRfl/classes/TrackingCamera.h> // for cursor->yaw/pitch calculation

#include <odCore/LevelObject.h>
#include <odCore/Level.h>
#include <odCore/Client.h>
#include <odCore/Server.h>

#include <odCore/rfl/Rfl.h>
#include <odCore/rfl/PrefetchProbe.h>

#include <odCore/input/InputManager.h>
#include <odCore/input/Keys.h>

#include <odCore/audio/SoundSystem.h>

#include <odCore/physics/PhysicsSystem.h>

#include <odCore/render/Renderer.h>

namespace dragonRfl
{

    HumanControl_Sv::HumanControl_Sv(odNet::ClientId clientId)
    : mClientId(clientId)
    , mYaw(0)
	, mPitch(0)
    , mState(State::Idling)
    , mLastUpdatedYaw(0)
    {
    }

    HumanControl_Sv::~HumanControl_Sv()
    {
    }

    void HumanControl_Sv::onLoaded()
    {
        // prefetch referenced assets
        odRfl::PrefetchProbe probe(getLevelObject().getClass()->getAssetProvider());
        mFields.probeFields(probe);

        // configure controls FIXME: this handler is not memory safe because actions are not uniquely owned!
        auto actionHandler = std::bind(&HumanControl_Sv::_handleAction, this, std::placeholders::_1, std::placeholders::_2);

        auto &inputManager = getServer().getInputManagerForClient(mClientId);

        auto &forwardAction = inputManager.getAction(Action::Forward);
        forwardAction.setRepeatable(false);
        forwardAction.addCallback(actionHandler);

        auto &backwardAction = inputManager.getAction(Action::Backward);
        backwardAction.setRepeatable(false);
        backwardAction.addCallback(actionHandler);

        auto &attackAction = inputManager.getAction(Action::Attack_Primary);
        attackAction.setRepeatable(false);
        attackAction.addCallback(actionHandler);
        attackAction.setIgnoreUpEvents(true);
    }

    void HumanControl_Sv::onSpawned()
    {
        auto &obj = getLevelObject();

    	Logger::verbose() << "Spawned Human Control at "
    			<< obj.getPosition().x << "/"
				<< obj.getPosition().y << "/"
				<< obj.getPosition().z;

    	glm::vec3 playerLookDirection = glm::eulerAngles(obj.getRotation());
    	mPitch = playerLookDirection.x;
    	mYaw = playerLookDirection.y;

        odAnim::Skeleton *skeleton = obj.getOrCreateSkeleton();
        if(skeleton != nullptr)
        {
            mPhysicsHandle = getServer().getPhysicsSystem().createObjectHandle(obj, false);
            mCharacterController = std::make_unique<odPhysics::CharacterController>(getServer().getPhysicsSystem(), mPhysicsHandle, obj, 0.05, 0.3);

            mAnimPlayer = std::make_unique<odAnim::SkeletonAnimationPlayer>(skeleton);
            mAnimPlayer->setRootNodeAccumulator(mCharacterController.get());

            mAnimPlayer->setRootNodeAccumulationModes(odAnim::AxesModes{ odAnim::AccumulationMode::Bone,
                                                                         odAnim::AccumulationMode::Bone,
                                                                         odAnim::AccumulationMode::Bone
                                                                       });

            mAnimPlayer->playAnimation(mFields.readyAnim.getAsset(), odAnim::PlaybackType::Looping, 1.0f);

        }else
        {
            Logger::warn() << "Used Human Control class on object without skeleton";
        }

    	obj.setEnableUpdate(true);
    }

    void HumanControl_Sv::onUpdate(float relTime)
    {
        static const float turnAnimThreshold = glm::half_pi<float>(); // angular yaw speed at which turn animation is triggered (in rad/sec)

        getLevelObject().setRotation(glm::quat(glm::vec3(0, mYaw, 0)));

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
                _playAnim(mFields.turnLeft, true, false);
                mState = State::TurningLeft;

            }else if(yawSpeed <= -turnAnimThreshold)
            {
                _playAnim(mFields.turnRight, true, false);
                mState = State::TurningRight;

            }else if(mState != State::Idling)
            {
                if(mAnimPlayer != nullptr && mAnimPlayer->isPlaying())
                {
                    break; // wait till turn anim is done
                }

                _playAnim(mFields.readyAnim, true, false);
                mState = State::Idling;
            }
            break;

        default:
            break;
        }

        if(mCharacterController != nullptr)
        {
            mCharacterController->update(relTime);
        }

        if(mAnimPlayer != nullptr)
        {
            mAnimPlayer->update(relTime);
        }
    }

    void HumanControl_Sv::_handleAction(Action action, odInput::ActionState state)
    {
        if(state == odInput::ActionState::BEGIN)
        {
            switch(action)
            {
            case Action::Forward:
                _playAnim(mFields.runAnim, false, true);
                mState = State::RunningForward;
                break;

            case Action::Backward:
                _playAnim(mFields.runBackwards, false, true);
                mState = State::RunningBackward;
                break;

            case Action::Attack_Primary:
                _playAnim(mFields.oneHandRH, false, false);
                _attack();
                break;

            default:
                break;
            }

        }else
        {
            _playAnim(mFields.readyAnim, true, true);
            mState = State::Idling;
        }
    }

    void HumanControl_Sv::_attack()
    {
        Logger::info() << "Trying to attack...";

        auto &obj = getLevelObject();
        auto pos = obj.getPosition();
        odPhysics::ContactTestResultVector results;
        getServer().getPhysicsSystem().sphereTest(pos, 1.0, odPhysics::PhysicsTypeMasks::LevelObject, results);

        for(auto &result : results)
        {
            odPhysics::ObjectHandle *objectHandle = result.handle->asObjectHandle();
            if(objectHandle != nullptr)
            {
                Logger::info() << "I, " << obj.getObjectId() << ", attacked " << objectHandle->getLevelObject().getObjectId();
            }
        }
    }

    void HumanControl_Sv::_playAnim(const odRfl::AnimRef &animRef, bool skeletonOnly, bool looping)
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
            mAnimPlayer->playAnimation(animRef.getAsset(), playbackType, 1.0f);
            mAnimPlayer->setRootNodeAccumulationModes(skeletonOnly ? fixedAccum : walkAccum);
        }
    }


    HumanControl_Cl::HumanControl_Cl()
    : mYaw(0)
	, mPitch(0)
    {
    }

    HumanControl_Cl::~HumanControl_Cl()
    {
    }

    void HumanControl_Cl::onLoaded()
    {
        getLevelObject().setSpawnStrategy(od::SpawnStrategy::Always);

        mInputListener = getClient().getInputManager().createInputListener();
        mInputListener->setMouseMoveCallback(std::bind(&HumanControl_Cl::_handleCursorMovement, this, std::placeholders::_1));
    }

    void HumanControl_Cl::onSpawned()
    {
        auto &obj = getLevelObject();

    	mRenderHandle = getClient().getRenderer().createHandleFromObject(obj);
    }

    void HumanControl_Cl::onTransformChanged()
    {
        auto &obj = getLevelObject();

        odAudio::SoundSystem *soundSystem = getClient().getSoundSystem();
        if(soundSystem != nullptr)
        {
            glm::vec3 pos = obj.getPosition();

            glm::quat lookDirection = obj.getRotation();
            glm::vec3 at = lookDirection * glm::vec3(0, 0, -1);
            glm::vec3 up = glm::vec3(0, 1, 0);

            soundSystem->setListenerPosition(pos);
            soundSystem->setListenerOrientation(at, up);
        }

        if(mRenderHandle != nullptr)
        {
            std::lock_guard<std::mutex> lock(mRenderHandle->getMutex());

            mRenderHandle->setPosition(obj.getPosition());
            mRenderHandle->setOrientation(obj.getRotation());
            mRenderHandle->setScale(obj.getScale());
        }
    }

    void HumanControl_Cl::_handleCursorMovement(const glm::vec2 &cursorPos)
    {
        glm::vec2 yawPitch = TrackingCamera_Cl::cursorPosToYawPitch(cursorPos);
        mYaw = yawPitch.x;
        mPitch = yawPitch.y;
    }

}
