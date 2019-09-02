/*
 * DebugDrawer.h
 *
 *  Created on: 23 Feb 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_PHYSICS_BULLET_DEBUGDRAWER_H_
#define INCLUDE_ODCORE_PHYSICS_BULLET_DEBUGDRAWER_H_

#include <vector>

#include <glm/vec3.hpp>

#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>
#include <LinearMath/btIDebugDraw.h>

#include <odCore/RefCounted.h>

#include <odCore/render/Array.h>

namespace odRender
{
    class Renderer;
    class Handle;
    class Geometry;
}

namespace odBulletPhysics
{

    class DebugDrawer final : public btIDebugDraw
    {
    public:

        DebugDrawer(odRender::Renderer *renderer, btCollisionWorld *collisionWorld);
        virtual ~DebugDrawer();

        virtual void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) override;

        virtual void drawContactPoint(const btVector3 &pointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color) override;

        virtual void reportErrorWarning(const char *warningString) override;

        virtual void draw3dText(const btVector3 &location, const char *textString) override;

        virtual void setDebugMode(int debugMode) override;
        virtual int getDebugMode() const override;

        void update(float relTime);


    private:

        odRender::Renderer *mRenderer;
        btCollisionWorld *mCollisionWorld;
        int mDebugMode;

        od::RefPtr<odRender::Handle> mRenderHandle;
        od::RefPtr<odRender::Geometry> mGeometry;

        std::unique_ptr<odRender::ArrayAccessHandler<glm::vec3>> mVertexArray;
        std::unique_ptr<odRender::ArrayAccessHandler<glm::vec4>> mColorArray;

        bool mSingleShotUpdate;
        size_t mLastMaxVertexCount;
    };


}

#endif /* INCLUDE_ODCORE_PHYSICS_BULLET_DEBUGDRAWER_H_ */
