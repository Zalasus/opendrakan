/*
 * DebugDrawer.h
 *
 *  Created on: 23 Feb 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_PHYSICS_BULLET_DEBUGDRAWER_H_
#define INCLUDE_ODCORE_PHYSICS_BULLET_DEBUGDRAWER_H_

#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>
#include <LinearMath/btIDebugDraw.h>

namespace odRender
{
    class PhysicsDebugDrawer;
}

namespace odBulletPhysics
{

    class DebugDrawer : public btIDebugDraw
    {
    public:

        DebugDrawer(btCollisionWorld *collisionWorld, odRender::PhysicsDebugDrawer *rendererInterface);

        virtual void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) override;

        virtual void drawContactPoint(const btVector3 &pointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color) override;

        virtual void reportErrorWarning(const char *warningString) override;

        virtual void draw3dText(const btVector3 &location, const char *textString) override;

        virtual void setDebugMode(int debugMode) override;
        virtual int getDebugMode() const override;

        void update(float relTime);


    private:

        btCollisionWorld *mCollisionWorld;
        odRender::PhysicsDebugDrawer *mRendererInterface;
        int mDebugMode;

    };


}

#endif /* INCLUDE_ODCORE_PHYSICS_BULLET_DEBUGDRAWER_H_ */
