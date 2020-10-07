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

#include <odCore/net/IdTypes.h>

#include <odCore/anim/SkeletonAnimationPlayer.h>

#include <odCore/render/Handle.h>

#include <odCore/rfl/DummyClass.h>

#include <dragonRfl/classes/HumanControlFields.h>
#include <dragonRfl/LocalPlayer.h>
#include <dragonRfl/Actions.h>

namespace dragonRfl
{

	class HumanControl_Sv : public odRfl::ServerClass, public odRfl::SpawnableClass, public odRfl::ClassImpl<HumanControl_Sv>
	{
	public:

		HumanControl_Sv(odNet::ClientId clientId);
		virtual ~HumanControl_Sv();

        virtual odRfl::FieldBundle &getFields() override { return mFields; }

        virtual void onLoaded() override;
		virtual void onSpawned() override;
		virtual void onUpdate(float relTime) override;


	 private:

        enum class State
        {
            Idling,
            TurningLeft,
            TurningRight,
            RunningForward,
            RunningBackward
        };

        void _handleAction(Action action, odInput::ActionState state);
        void _handleAnalogAction(Action action, const glm::vec2 &pos);
        void _attack();
		void _playAnim(const odRfl::AnimRef &animRef, bool skeletonOnly, bool looping, float skipAhead = 0.0);

		HumanControlFields mFields;

        odNet::ClientId mClientId;
        float mYaw;
		float mPitch;
		State mState;
        float mLastUpdatedYaw;

		std::unique_ptr<odAnim::SkeletonAnimationPlayer> mAnimPlayer;

		std::unique_ptr<odPhysics::CharacterController> mCharacterController;
	};


    class HumanControl_Cl : public odRfl::ClientClass, public odRfl::SpawnableClass, public odRfl::ClassImpl<HumanControl_Cl>
	{
	public:

		HumanControl_Cl();
		virtual ~HumanControl_Cl();

        virtual odRfl::FieldBundle &getFields() override { return mFields; }

		virtual void onLoaded() override;
		virtual void onSpawned() override;
		virtual void onTransformChanged() override;
        virtual void onUpdate(float relTime) override;


	 private:

        void _handleAnalogAction(Action action, const glm::vec2 &pos);

		HumanControlFields mFields;

        std::unique_ptr<odAnim::SkeletonAnimationPlayer> mAnimPlayer;

	};


    /**
     * A dummy used to represent all HumanControls created by the server, i.e. other players.
     * HumanControl_Cl contains the actual controller code. A custom downlink message is used to
     * turn the object that represents the client in question into a HumanControl_Cl.
     */
    class HumanControlDummy_Cl : public odRfl::ClientClass, public odRfl::SpawnableClass, public odRfl::ClassImpl<HumanControlDummy_Cl>
	{
    public:

        HumanControlDummy_Cl();
		virtual ~HumanControlDummy_Cl();

        virtual odRfl::FieldBundle &getFields() override { return mFields; }

		virtual void onSpawned() override;


	 private:

		HumanControlFields mFields;

    };


    // server: never create. always needs special treatment
    // client: always some kind of rendered dummy. HumanControl_Cl creation is handled via a custom message
    using HumanControlFactory = odRfl::DefaultClassFactory<HumanControlFields, HumanControlDummy_Cl, odRfl::DummyClass>;


    OD_DEFINE_CLASS(HumanControl, 0x0009, "Player", "Human Control", HumanControlFactory);

}

#endif /* INCLUDE_RFL_DRAGON_HUMANCONTROL_H_ */
