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
    , mFrontIndex(0)
    , mBackIndex(1)
    , mDrawing(false)
    {
        mGeode = new osg::Geode();

        for(size_t i = 0; i < 2; ++i)
        {
            mGeometry[i] = new osg::Geometry();
            mGeometry[i]->setDataVariance(osg::Object::DYNAMIC);
            mGeometry[i]->setUseDisplayList(false);

            mDrawArrays[i] = new osg::DrawArrays(osg::PrimitiveSet::LINES);
            mGeometry[i]->addPrimitiveSet(mDrawArrays[i]);

            mVertexArray[i] = new osg::Vec3Array(128);
            mGeometry[i]->setVertexArray(mVertexArray[i]);

            mColorArray[i] = new osg::Vec4Array(128);
            mGeometry[i]->setColorArray(mColorArray[i]);

            mGeode->addDrawable(mGeometry[i]);
        }

        mSceneRoot->addChild(mGeode);
    }

    PhysicsDebugDrawer::~PhysicsDebugDrawer()
    {
        mSceneRoot->removeChild(mGeode);
    }

    void PhysicsDebugDrawer::startDrawing()
    {
        mVertexArray[mBackIndex]->clear();
        mColorArray[mBackIndex]->clear();

        mDrawing = true;
    }

    void PhysicsDebugDrawer::drawLine(const glm::vec3 &start, const glm::vec3 &end, const glm::vec3 &color)
    {
        if(!mDrawing)
        {
            return;
        }

        mVertexArray[mBackIndex]->push_back(GlmAdapter::toOsg(start));
        mVertexArray[mBackIndex]->push_back(GlmAdapter::toOsg(end));

        osg::Vec4 colorAlpha(GlmAdapter::toOsg(color), 1.0);
        mColorArray[mBackIndex]->push_back(colorAlpha);
        mColorArray[mBackIndex]->push_back(colorAlpha);
    }

    void PhysicsDebugDrawer::endDrawing()
    {
        mDrawing = false;

        mDrawArrays[mBackIndex]->setCount(mVertexArray[mBackIndex]->size());
        mVertexArray[mBackIndex]->dirty();
        mColorArray[mBackIndex]->dirty();
        mGeometry[mBackIndex]->dirtyBound();

        mGeode->addDrawable(mGeometry[mBackIndex]);
        mGeode->removeDrawable(mGeometry[mFrontIndex]);

        std::swap(mBackIndex, mFrontIndex);
    }

}


