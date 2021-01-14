/*
 * DebugDrawer.cpp
 *
 *  Created on: 24 Feb 2019
 *      Author: zal
 */

#include <odCore/physics/bullet/DebugDrawer.h>

#include <cassert>

#include <odCore/Logger.h>
#include <odCore/Panic.h>

#include <odCore/physics/bullet/BulletAdapter.h>

#include <odCore/render/Renderer.h>
#include <odCore/render/Handle.h>
#include <odCore/render/Geometry.h>
#include <odCore/render/Model.h>

namespace odBulletPhysics
{

    DebugDrawer::DebugDrawer(odRender::Renderer &renderer, btCollisionWorld *collisionWorld)
    : mCollisionWorld(collisionWorld)
    , mDebugMode(0)
    , mSingleShotUpdate(false)
    , mLastMaxVertexCount(0)
    {
        OD_CHECK_ARG_NONNULL(collisionWorld);

        mGeometry = renderer.createGeometry(odRender::PrimitiveType::LINES, false);

        mRenderHandle = renderer.createHandle(odRender::RenderSpace::LEVEL);

        auto model = renderer.createModel();
        model->addGeometry(mGeometry);

        mRenderHandle->setModel(model);

        mCollisionWorld->setDebugDrawer(this);
    }

    DebugDrawer::~DebugDrawer()
    {
        mCollisionWorld->setDebugDrawer(nullptr);
    }

    void DebugDrawer::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
    {
        odRender::Array<glm::vec3> &vertices = mGeometry->getVertexArrayAccessHandler().getArray();
        odRender::Array<glm::vec4> &colors = mGeometry->getColorArrayAccessHandler().getArray();

        vertices.push_back(BulletAdapter::toGlm(from));
        vertices.push_back(BulletAdapter::toGlm(to));

        auto glmColor = glm::vec4(BulletAdapter::toGlm(color), 1.0);
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
            mSingleShotUpdate = false;

            odRender::ArrayAccessor<glm::vec3> vertices(mGeometry->getVertexArrayAccessHandler(), odRender::ArrayAccessMode::REPLACE);
            odRender::ArrayAccessor<glm::vec4> colors(mGeometry->getColorArrayAccessHandler(), odRender::ArrayAccessMode::REPLACE);

            vertices.clear();
            colors.clear();

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
        if(mDebugMode != btIDebugDraw::DBG_NoDebug)
        {
            if(mSingleShotUpdate)
            {
                return;
            }
            mSingleShotUpdate = true;

            auto &vertexArray = mGeometry->getVertexArrayAccessHandler();
            auto &colorArray = mGeometry->getColorArrayAccessHandler();

            vertexArray.acquire(false);
            colorArray.acquire(false);

            vertexArray.getArray().reserve(mLastMaxVertexCount);
            colorArray.getArray().reserve(mLastMaxVertexCount);

            mCollisionWorld->debugDrawWorld();

            size_t vertCount = vertexArray.getArray().size();
            mLastMaxVertexCount = std::max(vertCount, mLastMaxVertexCount);

            vertexArray.release(true);
            colorArray.release(true);
        }
    }

}
