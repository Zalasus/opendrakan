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
#include <odCore/render/Model.h>

namespace odBulletPhysics
{

    DebugDrawer::DebugDrawer(odRender::Renderer *renderer, btCollisionWorld *collisionWorld)
    : mRenderer(renderer)
    , mCollisionWorld(collisionWorld)
    , mDebugMode(0)
    , mVertexArray(nullptr)
    , mColorArray(nullptr)
    , mSingleShotUpdate(false)
    , mLastMaxVertexCount(0)
    {
        if(mCollisionWorld == nullptr)
        {
            throw od::Exception("Created Bullet debug drawer without a collision world");
        }

        mGeometry = renderer->createGeometry(odRender::PrimitiveType::LINES, false);
        mVertexArray = mGeometry->getVertexArrayAccessHandler();
        mColorArray = mGeometry->getColorArrayAccessHandler();

        mRenderHandle = renderer->createHandle(odRender::RenderSpace::LEVEL);

        auto model = renderer->createModel();
        model->addGeometry(mGeometry);

        {
            std::lock_guard<std::mutex> lock(mRenderHandle->getMutex());
            mRenderHandle->setModel(model);
        }

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

        odRender::Array<glm::vec3> &vertices = mVertexArray->getArray();
        odRender::Array<glm::vec4> &colors = mColorArray->getArray();

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

            mVertexArray->acquire();
            mColorArray->acquire();

            mVertexArray->getArray().clear();
            mColorArray->getArray().clear();

            mVertexArray->release();
            mColorArray->release();

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

            mVertexArray->acquire();
            mColorArray->acquire();

            mVertexArray->getArray().reserve(mLastMaxVertexCount);
            mColorArray->getArray().reserve(mLastMaxVertexCount);

            mCollisionWorld->debugDrawWorld();

            size_t vertCount = mVertexArray->getArray().size();
            mLastMaxVertexCount = std::max(vertCount, mLastMaxVertexCount);

            mVertexArray->release();
            mColorArray->release();
        }
    }

}
