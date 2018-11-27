/*
 * LightState.cpp
 *
 *  Created on: 27 Nov 2018
 *      Author: zal
 */

#include <odOsg/LightState.h>

#include <osg/NodeVisitor>
#include <osgUtil/CullVisitor>

#include <odCore/LevelObject.h>
#include <odCore/Layer.h>
#include <odCore/Exception.h>
#include <odCore/OdDefines.h>

#include <odCore/render/Light.h>

#include <odOsg/Renderer.h>
#include <odOsg/GlmAdapter.h>

namespace odOsg
{

    LightStateAttribute::LightStateAttribute(Renderer *renderer)
    : mRenderer(renderer)
    {
        mLights.reserve(OD_MAX_LIGHTS);
    }

    LightStateAttribute::LightStateAttribute(const LightStateAttribute &l, const osg::CopyOp &copyOp)
    : StateAttribute(l, copyOp)
    , mRenderer(l.mRenderer)
    {
        mLights.reserve(OD_MAX_LIGHTS);
    }

    osg::Object *LightStateAttribute::cloneType() const
    {
        throw od::Exception("Can't cloneType LightStateAttribute");
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
        throw od::UnsupportedException("LightStateAttribute::compare is not yet implemented");
    }

    void LightStateAttribute::clearLightList()
    {
        mLights.clear();
    }

    void LightStateAttribute::addLight(odRender::Light *light)
    {
        if(mLights.size() < OD_MAX_LIGHTS)
        {
            mLights.push_back(od::RefPtr<odRender::Light>(light));
        }
    }

    void LightStateAttribute::apply(osg::State &state) const
    {
        const osg::Matrix &viewMatrix = state.getInitialViewMatrix();

        mRenderer->applyLayerLight(viewMatrix, mLayerLightDiffuse, mLayerLightAmbient, mLayerLightDirection);

        auto it = mLights.begin();
        for(size_t i = 0; i < OD_MAX_LIGHTS; ++i)
        {
            if(it != mLights.end())
            {
                mRenderer->applyToLightUniform(viewMatrix, *it, i);
                ++it;

            }else
            {
                mRenderer->applyNullLight(i);
            }
        }
    }



    LightStateCallback::LightStateCallback(Renderer *renderer, osg::Node *node, bool ignoreCulledState)
    : mRenderer(renderer)
    , mIgnoreCulledState(ignoreCulledState)
    , mLightingDirty(true)
    {
        if(node == nullptr)
        {
            throw od::InvalidArgumentException("Passed null node to LightStateCallback()");
        }

        osg::StateSet *ss = node->getOrCreateStateSet();
        // TODO: perhaps remove all light-type attributes from state set here?

        osg::ref_ptr<LightStateAttribute> lightState(new LightStateAttribute(mRenderer));
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
        od::BoundingSphere bound(GlmAdapter::toGlm(node->getBound()._center), node->getBound()._radius);
        mRenderer->getLightsIntersectingSphere(bound, mTmpAffectingLightsList);

        glm::vec3 nodeCenter = GlmAdapter::toGlm(node->getBound().center());
        auto pred = [&nodeCenter](odRender::Light *l, odRender::Light *r){ return l->distanceToPoint(nodeCenter) < r->distanceToPoint(nodeCenter); };
        std::sort(mTmpAffectingLightsList.begin(), mTmpAffectingLightsList.end(), pred);

        for(auto it = mTmpAffectingLightsList.begin(); it != mTmpAffectingLightsList.end(); ++it)
        {
            mLightStateAttribute->addLight(*it); // will ignore all calls past maximum number of lights
        }

        mLightingDirty = false;
    }

}
