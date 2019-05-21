/*
 * DebugDrawer.cpp
 *
 *  Created on: 24 Feb 2019
 *      Author: zal
 */

#include <odCore/physics/bullet/DebugDrawer.h>

#include <cassert>

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
    , mVertexArray(nullptr)
    , mColorArray(nullptr)
    {
        if(mCollisionWorld == nullptr)
        {
            throw od::Exception("Created Bullet debug drawer without a collision world");
        }

        mRenderHandle = renderer->createHandle(odRender::RenderSpace::LEVEL);

        mVertexArray = mGeometry->getVertexArrayAccessHandler();
        mColorArray = mGeometry->getColorArrayAccessHandler();

        mCollisionWorld->setDebugDrawer(this);
    }

    DebugDrawer::~DebugDrawer()
    {
        mCollisionWorld->setDebugDrawer(nullptr);
    }

    void DebugDrawer::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
    {
        assert(mVertexArray != nullptr);
        assert(mColorArray != nullptr);

        odRender::Array<glm::vec3> vertices = mVertexArray->getArray();
        odRender::Array<glm::vec4> colors = mColorArray->getArray();

        vertices.push_back(BulletAdapter::toGlm(from));
        vertices.push_back(BulletAdapter::toGlm(to));

        auto glmColor = BulletAdapter::toGlm(color);
        colors.push_back(glmColor);
        colors.push_back(glmColor);
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
        mVertexArray->acquire();
        mColorArray->acquire();

        mCollisionWorld->debugDrawWorld();

        mVertexArray->release();
        mColorArray->release();
    }

}
