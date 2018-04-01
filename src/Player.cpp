/*
 * Player.cpp
 *
 *  Created on: 26 Feb 2018
 *      Author: zal
 */

#include "Player.h"

namespace od
{

	Player::Player()
	: mYaw(0)
	, mPitch(0)
	, mForwardSpeed(0)
	, mRightSpeed(0)
	, mLastSimTime(0)
	{
	}

    osg::Vec3f Player::getPosition()
    {
    	if(getLevelObject() == nullptr)
    	{
    		return osg::Vec3f(0,0,0);
    	}

    	return getLevelObject()->getPosition();
    }

    void Player::update(double simTime)
    {
    	if(mPlayerObject == nullptr)
    	{
    		return;
    	}

    	double relTime = simTime - mLastSimTime;

    	osg::Vec3f pos = mPlayerObject->getPosition();
    	pos += osg::Quat(mYaw, osg::Vec3f(0, 1, 0)) * osg::Vec3(1, 0, 0) * mForwardSpeed * relTime * 1.1 * 10; // 1.1 = default run speed
    	pos += osg::Quat(mYaw, osg::Vec3f(0, 1, 0)) * osg::Vec3(0, 0, 1) * mRightSpeed * relTime * 0.5 * 10; // 0.4 = default side strafe speed

    	mPlayerObject->setPosition(pos);

    	mLastSimTime = simTime;
    }
}
