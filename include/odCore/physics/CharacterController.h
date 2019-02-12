/*
 * CharacterController.h
 *
 *  Created on: May 23, 2018
 *      Author: zal
 */

#ifndef INCLUDE_PHYSICS_CHARACTERCONTROLLER_H_
#define INCLUDE_PHYSICS_CHARACTERCONTROLLER_H_

#include <memory>

#include <odCore/anim/MotionAccumulator.h>

namespace od
{
    class LevelObject;
}

namespace odPhysics
{

	class PhysicsSystem;

	class CharacterController : public odAnim::MotionAccumulator
	{
	public:

		CharacterController(od::LevelObject &charObject, float radius, float height);

		// implement odAnim::MotionAccumulator
        virtual void moveRelative(const glm::vec3 &relTranslation, float relTime) override;

		void update(float relTime);


	private:

		bool _step(float stepHeight); // returns true if object was hit during stepping

		od::LevelObject &mCharObject;
		PhysicsSystem &mPhysicsManager;
		glm::vec3 mCurrentPosition;
		glm::vec3 mUp;
		glm::vec3 mRelativeLowPoint;
		glm::vec3 mRelativeHighPoint;
		glm::vec3 mVelocity;
		glm::vec3 mDesiredDirection;
		glm::vec3 mDesiredPosition;

		bool mIsFalling;
		float mFallingVelocity;
	};

}

#endif /* INCLUDE_PHYSICS_CHARACTERCONTROLLER_H_ */
