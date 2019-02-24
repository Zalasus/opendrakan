/*
 * PhysicsDebugDrawer.cpp
 *
 *  Created on: 24 Feb 2019
 *      Author: zal
 */

#include <odOsg/render/PhysicsDebugDrawer.h>

#include <odOsg/GlmAdapter.h>

namespace odOsg
{

    PhysicsDebugDrawer::PhysicsDebugDrawer(osg::Group *sceneRoot)
    : mSceneRoot(sceneRoot)
    {
        mGeode = new osg::Geode();

        mGeometry = new osg::Geometry();
        mDrawArrays = new osg::DrawArrays(osg::PrimitiveSet::LINES);
        mGeometry->addPrimitiveSet(mDrawArrays);

        mVertexArray = new osg::Vec3Array(128);
        mGeometry->setVertexArray(mVertexArray);

        mColorArray = new osg::Vec4Array(128);
        mGeometry->setColorArray(mColorArray);

        mGeode->addDrawable(mGeometry);

        mSceneRoot->addChild(mGeode);
    }

    PhysicsDebugDrawer::~PhysicsDebugDrawer()
    {
        mSceneRoot->removeChild(mGeode);
    }

    void PhysicsDebugDrawer::startDrawing()
    {
        mVertexArray->clear();
        mColorArray->clear();
    }

    void PhysicsDebugDrawer::drawLine(const glm::vec3 &start, const glm::vec3 &end, const glm::vec3 &color)
    {
        mVertexArray->push_back(GlmAdapter::toOsg(start));
        mVertexArray->push_back(GlmAdapter::toOsg(end));

        osg::Vec4 colorAlpha(GlmAdapter::toOsg(color), 1.0);
        mColorArray->push_back(colorAlpha);
        mColorArray->push_back(colorAlpha);
    }

    void PhysicsDebugDrawer::endDrawing()
    {
        mDrawArrays->setCount(mVertexArray->size());
        mVertexArray->dirty();
        mColorArray->dirty();
        mGeometry->dirtyBound();
    }

}


