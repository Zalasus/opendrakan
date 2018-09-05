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
#include "OdDefines.h"
#include "light/LightManager.h"

namespace od
{

    LightStateAttribute::LightStateAttribute()
    : mNullLight(new osg::Light)
    {
        mNullLight->setDiffuse(osg::Vec4(0.0, 0.0, 0.0, 0.0));
        mNullLight->setAmbient(osg::Vec4(0.0, 0.0, 0.0, 0.0));
        mNullLight->setSpecular(osg::Vec4(0.0, 0.0, 0.0, 0.0));
        mNullLight->setPosition(osg::Vec4(1.0, 0.0, 0.0, 0.0));

        mLights.reserve(OD_MAX_LIGHTS);
    }

    LightStateAttribute::LightStateAttribute(const LightStateAttribute &l, const osg::CopyOp &copyOp)
    : StateAttribute(l, copyOp)
    , mNullLight(new osg::Light(*l.mNullLight, copyOp))
    {
        mLights.reserve(OD_MAX_LIGHTS);
    }

    int LightStateAttribute::compare(const StateAttribute& sa) const
    {
        throw UnsupportedException("LightStateAttribute::compare is not yet implemented");
    }

    void LightStateAttribute::clearLightList()
    {
        mLights.clear();
    }

    void LightStateAttribute::addLight(LightHandle *lightHandle)
    {
        if(mLights.size() < OD_MAX_LIGHTS)
        {
            mLights.push_back(lightHandle);
        }
    }

    void LightStateAttribute::apply(osg::State &state) const
    {
        auto it = mLights.begin();
        for(size_t i = 0; i < OD_MAX_LIGHTS; ++i)
        {
            if(it != mLights.end())
            {
                LightHandle *l = *it;
                ++it;
                if(l == nullptr || l->getLight() == nullptr)
                {
                    continue;
                }

                l->getLight()->setLightNum(i);
                l->getLight()->apply(state);

            }else
            {
                mNullLight->setLightNum(i);
                mNullLight->apply(state);
            }
        }
    }



    LightStateCallback::LightStateCallback(LightManager &lm, LevelObject &obj)
    : mLightManager(lm)
    , mLevelObject(obj)
    , mLightingDirty(true)
    {
        osg::StateSet *ss = mLevelObject.getOrCreateStateSet();
        // TODO: perhaps remove all light-type attributes from state set here?

        osg::ref_ptr<LightStateAttribute> lightState(new LightStateAttribute);
        mLightStateAttribute = lightState.get();
        ss->setAttribute(lightState, osg::StateAttribute::ON);

        mAffectingLightsCache.reserve(OD_MAX_LIGHTS);
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
        osg::Vec3 point = mLevelObject.getPosition();

        mAffectingLightsCache.clear();
        mLightManager.getLightsAffectingPoint(point, mAffectingLightsCache);

        auto pred = [&point](LightHandle *l, LightHandle *r){ return l->distanceToPoint(point) < r->distanceToPoint(point); };
        std::sort(mAffectingLightsCache.begin(), mAffectingLightsCache.end(), pred);

        mLightStateAttribute->clearLightList();
        for(auto it = mAffectingLightsCache.begin(); it != mAffectingLightsCache.end(); ++it)
        {
            mLightStateAttribute->addLight(*it); // will ignore all calls past maximum number of lights
        }

        mLightingDirty = false;
    }



}


