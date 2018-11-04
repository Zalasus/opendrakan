/*
 * BulletAdapter.h
 *
 *  Created on: 17 Apr 2018
 *      Author: zal
 */

#ifndef BULLETADAPTER_H_
#define BULLETADAPTER_H_

#include <LinearMath/btVector3.h>
#include <LinearMath/btTransform.h>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace odPhysics
{

    class BulletAdapter
    {
    public:

        inline static btVector3 toBullet(const glm::vec3 &v)
        {
            return btVector3(v.x, v.y, v.z);
        }

        inline static glm::vec3 toGlm(const btVector3 &v)
        {
            return glm::vec3(v.getX(), v.getY(), v.getZ());
        }

        inline static btQuaternion toBullet(const glm::quat &q)
        {
            return btQuaternion(q.x, q.y, q.z, q.w);
        }

        inline static glm::quat toGlm(const btQuaternion &q)
        {
            return glm::quat(q.getX(), q.getY(), q.getZ(), q.getW());
        }

        inline static btTransform makeBulletTransform(const glm::vec3 &translation, const glm::quat &rotation)
        {
            return btTransform(toBullet(rotation), toBullet(translation));
        }

    };

}

#endif /* BULLETADAPTER_H_ */
