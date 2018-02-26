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

    	Player() = default;
    	Player(const Player &p) = delete;
    	Player(Player &p) = delete;

    	inline void setLevelObject(LevelObjectPtr obj) { mPlayerObject = obj; }
    	inline LevelObjectPtr getLevelObject() { return mPlayerObject; }

    	void moveForward(float speed);


    private:

    	LevelObjectPtr mPlayerObject;
    };

}


#endif /* PLAYER_H_ */
