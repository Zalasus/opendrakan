/*
 * DebugDrawer.cpp
 *
 *  Created on: 24 Feb 2019
 *      Author: zal
 */

#include <odCore/physics/bullet/DebugDrawer.h>

#include <odCore/Exception.h>
#include <odCore/Logger.h>

#include <odCore/physics/bullet/BulletAdapter.h>

#include <odCore/render/Renderer.h>
#include <odCore/render/Handle.h>
#include <odCore/render/Geometry.h>

namespace odBulletPhysics
{

    DebugDrawer::DebugDrawer(odRender::Renderer *renderer, btCollisionWorld *collisionWorld)
    : mRenderer(renderer)
    , mCollisionWorld(collisionWorld)
    , mDebugMode(0)
    {
        if(mCollisionWorld == nullptr)
        {
            throw od::Exception("Created Bullet debug drawer without a collision world");
        }

        mRenderHandle = renderer->createHandle(odRender::RenderSpace::LEVEL);

        mCollisionWorld->setDebugDrawer(this);
    }

    DebugDrawer::~DebugDrawer()
    {
        mCollisionWorld->setDebugDrawer(nullptr);
    }

    void DebugDrawer::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
    {
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

        if(mDebugMode == btIDebugDraw::DBG_NoDebug)
        {

        }else
        {
        }
    }

    int DebugDrawer::getDebugMode() const
    {
        return mDebugMode;
    }

    void DebugDrawer::update(float relTime)
    {
        mCollisionWorld->debugDrawWorld();
    }

}
