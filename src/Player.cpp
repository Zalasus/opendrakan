/*
 * Player.cpp
 *
 *  Created on: 26 Feb 2018
 *      Author: zal
 */

#include "Player.h"

namespace od
{

    void Player::moveForward(float speed)
    {
    	if(mPlayerObject == nullptr)
    	{
    		return;
    	}

    	osg::Vec3f pos = mPlayerObject->getPosition();
    	pos += osg::Vec3(1, 0, 0) * speed;

    	mPlayerObject->setPosition(pos);
    }

    osg::Vec3f Player::getPosition()
    {
    	if(getLevelObject() == nullptr)
    	{
    		throw Exception("Can't get position. Player not yet spawned");
    	}

    	return getLevelObject()->getPosition();
    }
}
