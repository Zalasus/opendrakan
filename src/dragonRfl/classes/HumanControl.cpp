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

#include <odCore/physics/PhysicsSystem.h>

#include <odCore/render/Renderer.h>

#include <dragonRfl/RflDragon.h>

namespace dragonRfl
{

    HumanControl::HumanControl()
    : mYaw(0)
	, mPitch(0)
    , mState(State::Idling)
    , mLastUpdatedYaw(0)
    {
    }

    HumanControl::~HumanControl()
    {
    }

    void HumanControl::probeFields(odRfl::FieldProbe &probe)
    {
        PlayerCommon::probeFields(probe);
        mFields.probeFields(probe);
    }

    void HumanControl::onLoaded()
    {
        auto &obj = getLevelObject();
        auto &rfl = getRflAs<DragonRfl>();

        if(rfl.getLocalPlayer() != nullptr)
        {
            Logger::warn() << "Duplicate HumanControl objects found in level. Destroying duplicate";
            obj.requestDestruction();
            return;
        }

        obj.setSpawnStrategy(od::SpawnStrategy::Always);

        rfl.setLocalPlayer(this);

        // prefetch referenced assets
        odRfl::PrefetchProbe probe(getLevelObject().getClass()->getAssetProvider());
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

    void HumanControl::onSpawned()
    {
        auto &obj = getLevelObject();

    	Logger::verbose() << "Spawned Human Control at "
    			<< obj.getPosition().x << "/"
				<< obj.getPosition().y << "/"
				<< obj.getPosition().z;

    	glm::vec3 playerLookDirection = glm::eulerAngles(obj.getRotation());
    	mPitch = playerLookDirection.x;
    	mYaw = playerLookDirection.y;

    	odRender::Renderer *renderer = obj.getLevel().getEngine().getRenderer();
    	if(renderer == nullptr)
    	{
    	    return;
    	}
    	mRenderHandle = renderer->createHandleFromObject(obj);

        odAnim::Skeleton *skeleton = obj.getOrCreateSkeleton();
        if(skeleton != nullptr)
        {
            //mPhysicsHandle = obj.getLevel().getEngine().getPhysicsSystem().createObjectHandle(obj);
            mCharacterController = std::make_unique<odPhysics::CharacterController>(mPhysicsHandle, obj, 0.05, 0.3);

            mAnimPlayer = std::make_unique<odAnim::SkeletonAnimationPlayer>(skeleton);
            mAnimPlayer->setRootNodeAccumulator(mCharacterController.get());

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

    void HumanControl::onUpdate(float relTime)
    {
        static const float turnAnimThreshold = M_PI/2; // angular yaw speed at which turn animation is triggered (in rad/sec)

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
                _playAnim(mFields.mTurnLeft, true, false);
                mState = State::TurningLeft;

            }else if(yawSpeed <= -turnAnimThreshold)
            {
                _playAnim(mFields.mTurnRight, true, false);
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

        if(mCharacterController != nullptr)
        {
            mCharacterController->update(relTime);
        }

        if(mAnimPlayer != nullptr)
        {
            bool skeletonChanged = mAnimPlayer->update(relTime);

            if(skeletonChanged && mRenderHandle != nullptr)
            {
                getLevelObject().getOrCreateSkeleton()->flatten(mRenderHandle->getRig());
            }
        }
    }

    void HumanControl::onTransformChanged()
    {
        auto &obj = getLevelObject();

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

        if(mRenderHandle != nullptr)
        {
            std::lock_guard<std::mutex> lock(mRenderHandle->getMutex());

            mRenderHandle->setPosition(obj.getPosition());
            mRenderHandle->setOrientation(obj.getRotation());
            mRenderHandle->setScale(obj.getScale());
        }
    }

	glm::vec3 HumanControl::getPosition()
    {
    	return getLevelObject().getPosition();
    }

	od::LevelObject &HumanControl::getLevelObject()
	{
	    return LevelObjectClassBase::getLevelObject();
	}

	odPhysics::Handle *HumanControl::getPhysicsHandle()
	{
	    return nullptr;
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
                _playAnim(mFields.mRunBackwards, false, true);
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
            mAnimPlayer->playAnimation(animRef.getAsset(), playbackType, 1.0f);
            mAnimPlayer->setRootNodeAccumulationModes(skeletonOnly ? fixedAccum : walkAccum);
        }
    }

}
