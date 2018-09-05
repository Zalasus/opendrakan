/*
 * LightState.cpp
 *
 *  Created on: Sep 4, 2018
 *      Author: zal
 */

#include "light/LightState.h"

#include <osg/NodeVisitor>
#include <osgUtil/CullVisitor>

#include "LevelObject.h"
#include "Exception.h"

namespace od
{


    int LightStateAttribute::compare(const StateAttribute& sa) const
    {
        throw UnsupportedException("LightStateAttribute::compare is not yet implemented");
    }

    void LightStateAttribute::addLight(LightHandle *lightHandle)
    {

    }

    void LightStateAttribute::apply(osg::State &state) const
    {

    }




    LightStateCallback::LightStateCallback(LightManager &lm, LevelObject &obj)
    : mLightManager(lm)
    , mLevelObject(obj)
    , mLightingDirty(true)
    {
        osg::ref_ptr<LightStateAttribute> lightState(new LightStateAttribute);
        mLightStateAttribute = lightState.get();
        mLevelObject.getOrCreateStateSet()->setAttribute(lightState, osg::StateAttribute::ON);
    }

    void LightStateCallback::operator()(osg::Node *node, osg::NodeVisitor *nv)
    {
        traverse(node, nv);

        if(!mLightingDirty)
        {
            return;
        }

        if(node == nullptr || nv == nullptr || nv->getVisitorType() != osg::NodeVisitor::CULL_VISITOR)
        {
            return;
        }

        osgUtil::CullVisitor *cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
        if(cv == nullptr || cv->isCulled(*node))
        {
            return; // no need to update light state on a node that is not visible
        }

        _updateLightState();
    }

    void LightStateCallback::_updateLightState()
    {


        mLightingDirty = false;
    }



}


