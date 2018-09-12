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

    LightStateAttribute::LightStateAttribute(LightManager &lm)
    : mLightManager(lm)
    {
        mLights.reserve(OD_MAX_LIGHTS);
    }

    LightStateAttribute::LightStateAttribute(const LightStateAttribute &l, const osg::CopyOp &copyOp)
    : StateAttribute(l, copyOp)
    , mLightManager(l.mLightManager)
    {
        mLights.reserve(OD_MAX_LIGHTS);
    }

    osg::Object *LightStateAttribute::cloneType() const
    {
        throw Exception("Can't cloneType LightStateAttribute");
    }

    osg::Object *LightStateAttribute::clone(const osg::CopyOp& copyop) const
    {
        return new LightStateAttribute(*this, copyop);
    }

    bool LightStateAttribute::isSameKindAs(const osg::Object* obj) const
    {
        return dynamic_cast<const LightStateAttribute*>(obj) != nullptr;
    }

    bool LightStateAttribute::getModeUsage(ModeUsage &usage) const
    {
        return true; // completely shader based. doesn't use any modes
    }

    int LightStateAttribute::compare(const StateAttribute& sa) const
    {
        throw UnsupportedException("LightStateAttribute::compare is not yet implemented");
    }

    void LightStateAttribute::clearLightList()
    {
        mLights.clear();
    }

    void LightStateAttribute::addLight(Light *light)
    {
        if(mLights.size() < OD_MAX_LIGHTS)
        {
            mLights.push_back(osg::ref_ptr<Light>(light));
        }
    }

    void LightStateAttribute::apply(osg::State &state) const
    {
        const osg::Matrix &viewMatrix = state.getInitialViewMatrix();

        mLightManager.applyLayerLight(viewMatrix, mLayerLightDiffuse, mLayerLightAmbient, mLayerLightDirection);

        auto it = mLights.begin();
        for(size_t i = 0; i < OD_MAX_LIGHTS; ++i)
        {
            if(it != mLights.end())
            {
                mLightManager.applyToLightUniform(viewMatrix, *it, i);
                ++it;

            }else
            {

                mLightManager.applyNullLight(i);
            }
        }
    }



    LightStateCallback::LightStateCallback(LightManager &lm, osg::Node *node, bool ignoreCulledState)
    : mLightManager(lm)
    , mIgnoreCulledState(ignoreCulledState)
    , mLightingDirty(true)
    {
        if(node == nullptr)
        {
            throw InvalidArgumentException("Passed null node to LightStateCallback()");
        }

        osg::StateSet *ss = node->getOrCreateStateSet();
        // TODO: perhaps remove all light-type attributes from state set here?

        osg::ref_ptr<LightStateAttribute> lightState(new LightStateAttribute(mLightManager));
        mLightStateAttribute = lightState.get();
        ss->setAttribute(lightState, osg::StateAttribute::ON);

        mTmpAffectingLightsList.reserve(OD_MAX_LIGHTS);
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
        if(cv == nullptr)
        {
            return; // was no cull visitor after all. ignore
        }

        if(!mIgnoreCulledState && cv->isCulled(*node))
        {
            return; // no need to update light state on a node that is not visible
        }

        _updateLightState(node);
    }

    void LightStateCallback::_updateLightState(osg::Node *node)
    {
        mLightStateAttribute->clearLightList();

        mTmpAffectingLightsList.clear();
        mLightManager.getLightsIntersectingSphere(node->getBound(), mTmpAffectingLightsList);

        osg::Vec3 nodeCenter = node->getBound().center();
        auto pred = [&nodeCenter](Light *l, Light *r){ return l->distanceToPoint(nodeCenter) < r->distanceToPoint(nodeCenter); };
        std::sort(mTmpAffectingLightsList.begin(), mTmpAffectingLightsList.end(), pred);

        for(auto it = mTmpAffectingLightsList.begin(); it != mTmpAffectingLightsList.end(); ++it)
        {
            mLightStateAttribute->addLight(*it); // will ignore all calls past maximum number of lights
        }

        mLightingDirty = false;
    }



}


