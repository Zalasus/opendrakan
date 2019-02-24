/*
 * DebugDrawer.cpp
 *
 *  Created on: 24 Feb 2019
 *      Author: zal
 */

#include <odCore/physics/bullet/DebugDrawer.h>

#include <odCore/Exception.h>
#include <odCore/Logger.h>

#include <odCore/render/PhysicsDebugDrawer.h>

#include <odCore/physics/bullet/BulletAdapter.h>

namespace odBulletPhysics
{

    DebugDrawer::DebugDrawer(btCollisionWorld *collisionWorld, odRender::PhysicsDebugDrawer *rendererInterface)
    : mCollisionWorld(collisionWorld)
    , mRendererInterface(rendererInterface)
    , mDebugMode(0)
    {
        if(mCollisionWorld == nullptr)
        {
            throw od::Exception("Created Bullet debug drawer without a collision world");
        }

        if(mRendererInterface == nullptr)
        {
            throw od::Exception("Created Bullet debug drawer without a renderer interface to draw to");
        }
    }

    void DebugDrawer::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
    {
        mRendererInterface->drawLine(BulletAdapter::toGlm(from), BulletAdapter::toGlm(to), BulletAdapter::toGlm(color));
    }

    void DebugDrawer::drawContactPoint(const btVector3 &pointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color)
    {
    }

    void DebugDrawer::reportErrorWarning(const char *warningString)
    {
        Logger::warn() << "Bullet warning: " << warningString;
    }

    void DebugDrawer::draw3dText(const btVector3 &location, const char *textString)
    {
    }

    void DebugDrawer::setDebugMode(int debugMode)
    {
        mDebugMode = debugMode;
    }

    int DebugDrawer::getDebugMode() const
    {
        return mDebugMode;
    }

    void DebugDrawer::update(float relTime)
    {
        mRendererInterface->startDrawing();
        mCollisionWorld->debugDrawWorld();
        mRendererInterface->endDrawing();
    }

}
