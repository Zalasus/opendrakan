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
#include <osg/Vec3>
#include <osg/Quat>

namespace od
{

    class BulletAdapter
    {
    public:

        inline static btVector3 toBullet(const osg::Vec3f &v)
        {
            return btVector3(v.x(), v.y(), v.z());
        }

        inline static osg::Vec3 toOsg(const btVector3 &v)
        {
            return osg::Vec3(v.getX(), v.getY(), v.getZ());
        }

        inline static btQuaternion toBullet(const osg::Quat &q)
        {
            return btQuaternion(q.x(), q.y(), q.z(), q.w());
        }

        inline static osg::Quat toOsg(const btQuaternion &q)
        {
            return osg::Quat(q.getX(), q.getY(), q.getZ(), q.getW());
        }

        inline static btTransform makeBulletTransform(const osg::Vec3f &translation, const osg::Quat &rotation)
        {
            return btTransform(toBullet(rotation), toBullet(translation));
        }

    };

}

#endif /* BULLETADAPTER_H_ */
