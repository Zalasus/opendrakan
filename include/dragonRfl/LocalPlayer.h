/*
 * LocalPlayer.h
 *
 *  Created on: 5 Nov 2018
 *      Author: zal
 */

#ifndef INCLUDE_DRAGONRFL_LOCALPLAYER_H_
#define INCLUDE_DRAGONRFL_LOCALPLAYER_H_

namespace dragonRfl
{

    class LocalPlayer
    {
    public:

        virtual ~LocalPlayer() = default;

        virtual float getYaw() const = 0;
        virtual void setYaw(float f) = 0;
        virtual float getPitch() const = 0;
        virtual void setPitch(float f) = 0;
        virtual void moveForward(float speed) = 0;
        virtual void moveRight(float speed) = 0;
        virtual glm::vec3 getPosition() = 0;
        virtual od::LevelObject &getLevelObject() = 0;

    };

}

#endif /* INCLUDE_DRAGONRFL_LOCALPLAYER_H_ */