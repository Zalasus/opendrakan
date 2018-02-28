/*
 * Player.h
 *
 *  Created on: 16 Feb 2018
 *      Author: zal
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#include "LevelObject.h"

namespace od
{

    class Player
    {
    public:

    	Player();
    	Player(const Player &p) = delete;
    	Player(Player &p) = delete;

    	inline void setLevelObject(LevelObjectPtr obj) { mPlayerObject = obj; }
    	inline LevelObjectPtr getLevelObject() { return mPlayerObject; }
    	inline float getYaw() const { return mYaw; }
    	inline float getPitch() const { return mPitch; }
    	inline void setYaw(float f) { mYaw = f; }
    	inline void setPitch(float f) { mPitch = f; }
    	inline void moveForward(float speed) { mForwardSpeed = speed; }
    	inline void moveRight(float speed) { mRightSpeed = speed; }

    	osg::Vec3f getPosition();
    	void update(double frameTime);


    private:

    	LevelObjectPtr mPlayerObject;
    	float mYaw;
    	float mPitch;
    	float mForwardSpeed;
    	float mRightSpeed;
    };

}


#endif /* PLAYER_H_ */
