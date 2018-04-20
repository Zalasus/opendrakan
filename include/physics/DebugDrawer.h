/*
 * DebugDrawer.h
 *
 *  Created on: 17 Apr 2018
 *      Author: zal
 */

#ifndef INCLUDE_PHYSICS_DEBUGDRAWER_H_
#define INCLUDE_PHYSICS_DEBUGDRAWER_H_

#include <LinearMath/btIDebugDraw.h>
#include <BulletDynamics/Dynamics/btDynamicsWorld.h>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Array>

namespace od
{

    /*
     * Took some inspirition from OpenMW. Thanks, guys!
     */
	class DebugDrawer : public btIDebugDraw
	{
	public:

	    DebugDrawer(osg::ref_ptr<osg::Group> parentNode, btDynamicsWorld *world);
        ~DebugDrawer();

        void step();

        virtual void drawLine(const btVector3& from,const btVector3& to,const btVector3& color) override;
        virtual void drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color) override;
        virtual void reportErrorWarning(const char* warningString) override;
        virtual void draw3dText(const btVector3& location,const char* textString) override;
        virtual void setDebugMode(int isOn) override;
        virtual int getDebugMode() const override;


	private:

        osg::ref_ptr<osg::Group> mParentNode;
        btDynamicsWorld *mDynamicsWorld;
        osg::ref_ptr<osg::Geode> mGeode;
        osg::ref_ptr<osg::Geometry> mGeometry;
        osg::ref_ptr<osg::DrawArrays> mDrawArrays;
        osg::ref_ptr<osg::Vec3Array> mVertices;
        bool mDebugOn;
	};

}

#endif /* INCLUDE_PHYSICS_DEBUGDRAWER_H_ */
