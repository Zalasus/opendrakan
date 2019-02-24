/*
 * PhysicsDebugDrawer.h
 *
 *  Created on: 24 Feb 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_RENDER_PHYSICSDEBUGDRAWER_H_
#define INCLUDE_ODOSG_RENDER_PHYSICSDEBUGDRAWER_H_

#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/PrimitiveSet>
#include <osg/Array>

#include <odCore/render/PhysicsDebugDrawer.h>

namespace odOsg
{

    class PhysicsDebugDrawer : public odRender::PhysicsDebugDrawer
    {
    public:

        PhysicsDebugDrawer(osg::Group *sceneRoot);
        virtual ~PhysicsDebugDrawer();

        virtual void startDrawing() override;
        virtual void drawLine(const glm::vec3 &start, const glm::vec3 &end, const glm::vec3 &color) override;
        virtual void endDrawing() override;


    private:

        osg::ref_ptr<osg::Group> mSceneRoot;

        osg::ref_ptr<osg::Geode> mGeode;

        // we maintain two geometries, each with their own arrays, so we can update one while the other one is drawn,
        //  then simply swap the two once the update is finished
        osg::ref_ptr<osg::Geometry> mGeometry[2];
        osg::ref_ptr<osg::DrawArrays> mDrawArrays[2];
        osg::ref_ptr<osg::Vec3Array> mVertexArray[2];
        osg::ref_ptr<osg::Vec4Array> mColorArray[2];

        size_t mFrontIndex;
        size_t mBackIndex;

        bool mDrawing;
    };

}

#endif /* INCLUDE_ODOSG_RENDER_PHYSICSDEBUGDRAWER_H_ */
