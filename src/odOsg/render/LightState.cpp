/*
 * LightState.cpp
 *
 *  Created on: 27 Nov 2018
 *      Author: zal
 */

#include <odOsg/render/LightState.h>

#include <osg/NodeVisitor>
#include <osgUtil/CullVisitor>

#include <odCore/LevelObject.h>
#include <odCore/Layer.h>
#include <odCore/Exception.h>

#include <odOsg/GlmAdapter.h>
#include <odOsg/render/Renderer.h>

namespace odOsg
{

    LightStateAttribute::LightStateAttribute(Renderer *renderer, size_t maxLightCount)
    : mRenderer(renderer)
    , mMaxLightCount(maxLightCount)
    {
        mLights.reserve(mMaxLightCount);
    }

    LightStateAttribute::LightStateAttribute(const LightStateAttribute &l, const osg::CopyOp &copyOp)
    : StateAttribute(l, copyOp)
    , mRenderer(l.mRenderer)
    , mMaxLightCount(l.mMaxLightCount)
    {
        mLights.reserve(mMaxLightCount);
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

    void LightStateAttribute::addLight(od::Light *light)
    {
        if(mLights.size() < mMaxLightCount)
        {
            mLights.emplace_back(light);
        }
    }

    void LightStateAttribute::removeLight(od::Light *light)
    {
        auto it = std::find(mLights.begin(), mLights.end(), light);
        if(it != mLights.end())
        {
            mLights.erase(it);
        }
    }

    void LightStateAttribute::apply(osg::State &state) const
    {
        const osg::Matrix &viewMatrix = state.getInitialViewMatrix();

        mRenderer->applyLayerLight(viewMatrix, mLayerLightDiffuse, mLayerLightAmbient, mLayerLightDirection);

        for(size_t i = 0; i < mMaxLightCount; ++i)
        {
            if(i < mLights.size() && mLights[i].isNonNull())
            {
                auto light = mLights[i].aquire();
                mRenderer->applyToLightUniform(viewMatrix, light, i);

            }else
            {
                mRenderer->applyNullLight(i);
            }
        }
    }

}
