/*
 * Player.h
 *
 *  Created on: 16 Feb 2018
 *      Author: zal
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#include <osg/Vec3>

namespace od
{

	/**
	 * @brief Interface for RFL class representing a player.
	 *
	 * This allows abstracting access to player-related functions between the engine and the RFL.
	 */
    class Player
    {
    public:

    	virtual ~Player() = default;

    	virtual float getYaw() const = 0;
    	virtual void setYaw(float f) = 0;
    	virtual float getPitch() const = 0;
    	virtual void setPitch(float f) = 0;
    	virtual void moveForward(float speed) = 0;
    	virtual void moveRight(float speed) = 0;
    	virtual osg::Vec3f getPosition() = 0;
    };

}


#endif /* PLAYER_H_ */
