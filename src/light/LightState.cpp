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
#include "Layer.h"
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

    bool LightStateAttribute::getModeUsage(ModeUsage &usage) const
    {
        for (size_t i = 0; i < mLights.size(); ++i)
        {
            usage.usesMode(GL_LIGHT0 + i);
        }

        return true;
    }

    int LightStateAttribute::compare(const StateAttribute& sa) const
    {
        throw UnsupportedException("LightStateAttribute::compare is not yet implemented");
    }

    void LightStateAttribute::clearLightList()
    {
        mLights.clear();
    }

    void LightStateAttribute::addLight(osg::Light *light)
    {
        if(mLights.size() < OD_MAX_LIGHTS)
        {
            mLights.push_back(osg::ref_ptr<osg::Light>(light));
        }
    }

    void LightStateAttribute::apply(osg::State &state) const
    {
        auto it = mLights.begin();
        for(size_t i = 0; i < OD_MAX_LIGHTS; ++i)
        {
            if(it != mLights.end())
            {
                osg::Light *l = *it;
                ++it;
                if(l == nullptr)
                {
                    // TODO: light no longer exists. erase it
                    continue;
                }

                l->setLightNum(i);
                l->apply(state);

            }else
            {
                mNullLight->setLightNum(i);
                mNullLight->apply(state);
            }
        }
    }



    LightStateCallback::LightStateCallback(LightManager &lm, osg::Node *node)
    : mLightManager(lm)
    , mLightingDirty(true)
    {
        if(node == nullptr)
        {
            throw InvalidArgumentException("Passed null node to LightStateCallback()");
        }

        osg::StateSet *ss = node->getOrCreateStateSet();
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

        _updateLightState(node);
    }

    void LightStateCallback::_updateLightState(osg::Node *node)
    {
        mLightStateAttribute->clearLightList();

        // if a fixed light is defined, always add it first
        if(mFixedLight != nullptr)
        {
            mLightStateAttribute->addLight(mFixedLight);
        }

        mAffectingLightsCache.clear();
        mLightManager.getLightsIntersectingSphere(node->getBound(), mAffectingLightsCache);

        osg::Vec3 nodeCenter = node->getBound().center();
        auto pred = [&nodeCenter](LightHandle *l, LightHandle *r){ return l->distanceToPoint(nodeCenter) < r->distanceToPoint(nodeCenter); };
        std::sort(mAffectingLightsCache.begin(), mAffectingLightsCache.end(), pred);

        for(auto it = mAffectingLightsCache.begin(); it != mAffectingLightsCache.end(); ++it)
        {
            mLightStateAttribute->addLight((*it)->getLight()); // will ignore all calls past maximum number of lights
        }

        mLightingDirty = false;
    }



}


