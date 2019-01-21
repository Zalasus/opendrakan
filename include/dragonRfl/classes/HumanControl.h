/*
 * HumanControl.h
 *
 *  Created on: 2 Apr 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_HUMANCONTROL_H_
#define INCLUDE_RFL_DRAGON_HUMANCONTROL_H_

#include <odCore/db/Animation.h>

#include <odCore/physics/CharacterController.h>

#include <odCore/input/Action.h>

#include <dragonRfl/classes/HumanControlFields.h>
#include <dragonRfl/LocalPlayer.h>
#include <dragonRfl/Actions.h>

namespace dragonRfl
{
    class DragonRfl;

	class HumanControl : public HumanControlFields, public LocalPlayer
	{
	public:

		HumanControl(DragonRfl &rfl);
		virtual ~HumanControl();

		virtual void onLoaded(od::LevelObject &obj) override;
		virtual void onSpawned(od::LevelObject &obj) override;
		virtual void onUpdate(od::LevelObject &obj, float relTime) override;
		virtual void onMoved(od::LevelObject &obj) override;

		virtual float getYaw() const override { return mYaw; }
		virtual void setYaw(float f) override { mYaw = f; }
		virtual float getPitch() const override { return mPitch; }
		virtual void setPitch(float f) override { mPitch = f; }
		virtual void moveForward(float speed) override;
		virtual void moveRight(float speed) override;
		virtual glm::vec3 getPosition() override;
		virtual od::LevelObject &getLevelObject() override;


	 private:

		void _handleMovementAction(odInput::ActionHandle<Action> *action, odInput::InputEvent event);

		DragonRfl &mRfl;

		float mYaw;
		float mPitch;
		float mPrevYaw;
		float mForwardSpeed;
		float mRightSpeed;
		od::LevelObject *mPlayerObject;

		od::RefPtr<odInput::ActionHandle<Action>> mForwardAction;
		od::RefPtr<odInput::ActionHandle<Action>> mBackwardAction;

	};

}

OD_DEFINE_RFLCLASS_TRAITS(dragonRfl::DragonRfl, 0x0009, "Player", "Human Control", dragonRfl::HumanControl);

#endif /* INCLUDE_RFL_DRAGON_HUMANCONTROL_H_ */
