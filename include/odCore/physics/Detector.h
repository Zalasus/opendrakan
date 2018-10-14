/*
 * Detector.h
 *
 *  Created on: 14 Oct 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_PHYSICS_DETECTOR_H_
#define INCLUDE_ODCORE_PHYSICS_DETECTOR_H_

#include <vector>
#include <memory>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletCollision/BroadphaseCollision/btCollisionAlgorithm.h>

namespace od
{
    class LevelObject;
}

namespace odPhysics
{

    class Detector
    {
    public:

        Detector(btCollisionWorld *collisionWorld, btCollisionShape *objectShape, od::LevelObject &obj);
        ~Detector();

        inline const std::vector<od::LevelObject*> &getIntersectingObjects() { return mIntersectingObjects; }

        void update();


    private:

        btCollisionWorld *mCollisionWorld;
        btCollisionShape *mObjectShape;
        od::LevelObject &mLevelObject;
        std::unique_ptr<btPairCachingGhostObject> mGhostObject;
        std::vector<od::LevelObject*> mIntersectingObjects;
        btManifoldArray mManifoldArray;
    };

}

#endif /* INCLUDE_ODCORE_PHYSICS_DETECTOR_H_ */
