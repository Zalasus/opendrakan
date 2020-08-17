/*
 * HumanControl.h
 *
 *  Created on: 2 Apr 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_HUMANCONTROL_H_
#define INCLUDE_RFL_DRAGON_HUMANCONTROL_H_

#include <odCore/db/Animation.h>

#include <odCore/physics/Handles.h>
#include <odCore/physics/CharacterController.h>

#include <odCore/input/Action.h>
#include <odCore/input/InputListener.h>

#include <odCore/net/ClientConnector.h>

#include <odCore/anim/SkeletonAnimationPlayer.h>

#include <odCore/render/Handle.h>

#include <odCore/rfl/DummyClass.h>

#include <dragonRfl/classes/HumanControlFields.h>
#include <dragonRfl/LocalPlayer.h>
#include <dragonRfl/Actions.h>

namespace dragonRfl
{
    class DragonRfl;


	class HumanControl_Sv : public odRfl::ServerClass, public odRfl::SpawnableClass, public odRfl::ClassImpl<HumanControl_Sv>
	{
	public:

		HumanControl_Sv(odNet::ClientId clientId);
		virtual ~HumanControl_Sv();

        virtual odRfl::FieldBundle &getFields() override { return mFields; }

        virtual void onLoaded() override;


	 private:

        void _handleAction(Action action, odInput::ActionState state);
        void _attack();

        odNet::ClientId mClientId;

		HumanControlFields mFields;

        std::shared_ptr<ActionHandle> mAttackAction;

	};


    class HumanControl_Cl : public odRfl::ClientClass, public odRfl::SpawnableClass, public odRfl::ClassImpl<HumanControl_Cl>, public LocalPlayer
	{
	public:

		HumanControl_Cl();
		virtual ~HumanControl_Cl();

        virtual odRfl::FieldBundle &getFields() override { return mFields; }

		virtual void onLoaded() override;
		virtual void onSpawned() override;
		virtual void onUpdate(float relTime) override;
		virtual void onTransformChanged() override;

		virtual float getYaw() const override { return mYaw; }
		virtual void setYaw(float f) override { mYaw = f; }
		virtual float getPitch() const override { return mPitch; }
		virtual void setPitch(float f) override { mPitch = f; }
		virtual glm::vec3 getPosition() override;
		virtual od::LevelObject &getLevelObject() override;
		virtual std::shared_ptr<odPhysics::Handle> getPhysicsHandle() override;


	 private:

		enum class State
		{
		    Idling,
		    TurningLeft,
		    TurningRight,
		    RunningForward,
		    RunningBackward
		};

		void _handleMovementAction(Action action, odInput::ActionState state);
		void _handleCursorMovement(const glm::vec2 &cursorPos);
		void _playAnim(const odRfl::AnimRef &animRef, bool skeletonOnly, bool looping);

		HumanControlFields mFields;

		float mYaw;
		float mPitch;

		State mState;
		float mLastUpdatedYaw;

		std::unique_ptr<odAnim::SkeletonAnimationPlayer> mAnimPlayer;

		std::shared_ptr<odRender::Handle> mRenderHandle;
		std::shared_ptr<odPhysics::ObjectHandle> mPhysicsHandle;
		std::unique_ptr<odPhysics::CharacterController> mCharacterController;

		std::shared_ptr<ActionHandle> mForwardAction;
		std::shared_ptr<ActionHandle> mBackwardAction;
        std::shared_ptr<ActionHandle> mAttackAction;
		std::shared_ptr<odInput::InputListener> mInputListener;

	};

    using HumanControlFactory = odRfl::DefaultClassFactory<HumanControlFields, HumanControl_Cl, odRfl::DummyClass>;

    OD_DEFINE_CLASS(HumanControl, 0x0009, "Player", "Human Control", HumanControlFactory);

}

#endif /* INCLUDE_RFL_DRAGON_HUMANCONTROL_H_ */
