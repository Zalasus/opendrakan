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
        osg::ref_ptr<osg::Geometry> mGeometry;
        osg::ref_ptr<osg::DrawArrays> mDrawArrays;
        osg::ref_ptr<osg::Vec3Array> mVertexArray;
        osg::ref_ptr<osg::Vec4Array> mColorArray;
    };

}

#endif /* INCLUDE_ODOSG_RENDER_PHYSICSDEBUGDRAWER_H_ */
