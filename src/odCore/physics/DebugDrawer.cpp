/*
 * DebugDrawer.cpp
 *
 *  Created on: 20 Apr 2018
 *      Author: zal
 */

#include <odCore/physics/DebugDrawer.h>

#include <osg/Geometry>

#include <odCore/physics/BulletAdapter.h>
#include <odCore/Logger.h>

namespace odPhysics
{

    DebugDrawer::DebugDrawer(osg::ref_ptr<osg::Group> parentNode, btDynamicsWorld *world)
    : mParentNode(parentNode)
    , mDynamicsWorld(world)
    , mDebugOn(false)
    , mCullingActive(false)
    {
        mGeode = new osg::Geode;

        mGeometry = new osg::Geometry;
        mVertices = new osg::Vec3Array(128);
        mGeometry->setVertexArray(mVertices);
        mDrawArrays = new osg::DrawArrays(osg::PrimitiveSet::LINES);
        mGeometry->addPrimitiveSet(mDrawArrays);
        mGeometry->setUseDisplayList(false);
        mGeometry->setDataVariance(osg::Object::DYNAMIC);

        mGeode->addDrawable(mGeometry);

        parentNode->addChild(mGeode);
    }

    DebugDrawer::~DebugDrawer()
    {
        mParentNode->removeChild(mGeode);
    }

    void DebugDrawer::step()
    {
        mVertices->clear();
        mDynamicsWorld->debugDrawWorld();
        mDrawArrays->setCount(mVertices->size());
        mVertices->dirty();
        mGeometry->dirtyBound();
    }

    void DebugDrawer::setCullingSphere(float radius, osg::Vec3f center)
    {
    	mCullingSphere.set(center, radius);
    	mCullingActive = (radius != 0);
    }

    void DebugDrawer::drawLine(const btVector3& from,const btVector3& to,const btVector3& color)
    {
    	osg::Vec3f oFrom = BulletAdapter::toOsg(from);
    	osg::Vec3f oTo   = BulletAdapter::toOsg(to);

    	if(mCullingActive)
    	{
    		if(!mCullingSphere.contains(oFrom) && !mCullingSphere.contains(oTo))
    		{
    			return;
    		}
    	}

        mVertices->push_back(oFrom);
        mVertices->push_back(oTo);
    }

    void DebugDrawer::drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color)
    {

    }

    void DebugDrawer::reportErrorWarning(const char* warningString)
    {
        Logger::warn() << "(Bullet warning) " << warningString;
    }

    void DebugDrawer::draw3dText(const btVector3& location, const char* textString)
    {

    }

    void DebugDrawer::setDebugMode(int isOn)
    {
        mDebugOn = isOn;
    }

    int DebugDrawer::getDebugMode() const
    {
        return mDebugOn ? btIDebugDraw::DBG_DrawWireframe : 0;
    }

}

