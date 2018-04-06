/*
 * HumanControl.cpp
 *
 *  Created on: 2 Apr 2018
 *      Author: zal
 */

#include "rfl/dragon/HumanControl.h"

#include "rfl/Rfl.h"
#include "LevelObject.h"
#include "Level.h"
#include "Player.h"

namespace odRfl
{


	class PlayerUpdateCallback : public osg::NodeCallback
	{
	public:

		PlayerUpdateCallback(HumanControl &hc)
		: mHumanControl(hc)
		{
		}

		virtual void operator()(osg::Node *node, osg::NodeVisitor *nv)
		{
			traverse(node, nv);

			if(nv->getFrameStamp() != nullptr)
			{
				mHumanControl.update(nv->getFrameStamp()->getSimulationTime());
			}
		}


	private:

		HumanControl &mHumanControl;

	};



	HumanControl::HumanControl()
	: mYaw(0)
	, mPitch(0)
	, mForwardSpeed(0)
	, mRightSpeed(0)
    {
    }

	HumanControl::~HumanControl()
	{
		if(mUpdateCallback != nullptr && mPlayerObject != nullptr)
		{
			mPlayerObject->removeUpdateCallback(mUpdateCallback);
		}
	}

    void HumanControl::probeFields(RflFieldProbe &probe)
    {
    	PlayerCommon::probeFields(probe);
    }

    void HumanControl::spawn(od::LevelObject &obj)
    {
    	mDbWalkAnim = obj.getClass()->getDatabase().getAnimationByRef(mWalkAnim);

    	obj.getLevel().setPlayer(this);
    	mPlayerObject = &obj;

    	Logger::verbose() << "Spawned Human Control at "
    			<< obj.getPosition().x() << "/"
				<< obj.getPosition().y() << "/"
				<< obj.getPosition().z();

    	mUpdateCallback = new PlayerUpdateCallback(*this);
    	obj.addUpdateCallback(mUpdateCallback);
    }

    void HumanControl::moveForward(float speed)
    {
    	mForwardSpeed = speed;
    }

	void HumanControl::moveRight(float speed)
    {
    	mRightSpeed = speed;
    }

	osg::Vec3f HumanControl::getPosition()
    {
    	if(mPlayerObject == nullptr)
    	{
    		return osg::Vec3f(0,0,0);
    	}

    	return mPlayerObject->getPosition();
    }

    void HumanControl::update(double simTime)
    {
    	_updateMotion();
    }

    void HumanControl::_updateMotion()
    {
    	osg::Vec3 velocity;
    	velocity += osg::Quat(mYaw, osg::Vec3f(0, 1, 0)) * osg::Vec3(1, 0, 0) * mForwardSpeed  * mRunSpeed;
    	velocity += osg::Quat(mYaw, osg::Vec3f(0, 1, 0)) * osg::Vec3(0, 0, 1) * mRightSpeed * mSideStrafeSpeed;

    	//mPlayerObject->getOrCreateMotionAnim()->setVelocity(velocity);

    	if(mForwardSpeed > 0)
    	{
    		od::SkeletonAnimationPlayer *ap = mPlayerObject->getSkeletonAnimationPlayer();
    		if(ap != nullptr && ap->getPlayState() == od::AnimationPlayState::STOPPED)
    		{
    			ap->setAnimation(mDbWalkAnim);
    			ap->setPlayState(od::AnimationPlayState::PLAYING);
    		}
    	}
    }

    OD_REGISTER_RFL_CLASS(0x0009, "Human Control", HumanControl);

}
