/*
 * DebugDrawer.cpp
 *
 *  Created on: 20 Apr 2018
 *      Author: zal
 */

#include "physics/DebugDrawer.h"

#include <osg/Geometry>

#include "physics/BulletAdapter.h"
#include "Logger.h"

namespace od
{

    DebugDrawer::DebugDrawer(osg::ref_ptr<osg::Group> parentNode, btDynamicsWorld *world)
    : mParentNode(parentNode)
    , mDynamicsWorld(world)
    , mDebugOn(false)
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

    void DebugDrawer::drawLine(const btVector3& from,const btVector3& to,const btVector3& color)
    {
        mVertices->push_back(BulletAdapter::toOsg(from));
        mVertices->push_back(BulletAdapter::toOsg(to));
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
        return mDebugOn;
    }

}

