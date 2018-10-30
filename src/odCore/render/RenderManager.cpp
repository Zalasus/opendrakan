/*
 * RenderManager.cpp
 *
 *  Created on: Oct 27, 2018
 *      Author: zal
 */

#include <odCore/render/RenderManager.h>

#include <algorithm>
#include <osg/Light>
#include <osg/Material>

#include <odCore/OdDefines.h>
#include <odCore/Exception.h>
#include <odCore/Logger.h>
#include <odCore/LevelObject.h>

namespace odRender
{

    RenderManager::RenderManager(od::Engine &engine, osg::Group *sceneRoot)
    : mEngine(engine)
    , mShaderFactory(OD_SHADER_SRC_PATH)
    , mLightingEnabled(true)
    {
        osg::StateSet *ss = sceneRoot->getOrCreateStateSet();

         // attach our default shaders to root node so we don't use the fixed function pipeline anymore
        osg::ref_ptr<osg::Program> defaultProgram = getShaderFactory().getProgram("default");
        ss->setAttribute(defaultProgram);

        mLayerLightDiffuse = new osg::Uniform("layerLightDiffuse", osg::Vec3(1.0, 1.0, 1.0));
        mLayerLightAmbient = new osg::Uniform("layerLightAmbient", osg::Vec3(1.0, 1.0, 1.0));
        mLayerLightDirection = new osg::Uniform("layerLightDirection", osg::Vec3(0.0, -1.0, 0.0));
        ss->addUniform(mLayerLightDiffuse);
        ss->addUniform(mLayerLightAmbient);
        ss->addUniform(mLayerLightDirection);

        mLightDiffuseColors = new osg::Uniform(osg::Uniform::FLOAT_VEC3, "objectLightDiffuse", OD_MAX_LIGHTS);
        mLightIntensities = new osg::Uniform(osg::Uniform::FLOAT, "objectLightIntensity", OD_MAX_LIGHTS);
        mLightRadii = new osg::Uniform(osg::Uniform::FLOAT, "objectLightRadius", OD_MAX_LIGHTS);
        mLightPositions = new osg::Uniform(osg::Uniform::FLOAT_VEC3, "objectLightPosition", OD_MAX_LIGHTS);
        ss->addUniform(mLightDiffuseColors);
        ss->addUniform(mLightIntensities);
        ss->addUniform(mLightRadii);
        ss->addUniform(mLightPositions);
    }

    void RenderManager::addLight(Light *light)
    {
        light->addObserver(this);

        mLights.push_back(light);
    }

    void RenderManager::removeLight(Light *lightHandle)
    {
        auto it = std::find(mLights.begin(), mLights.end(), lightHandle);
        if(it != mLights.end())
        {
            mLights.erase(it);
        }
    }

    void RenderManager::getLightsAffectingPoint(const osg::Vec3 &point, std::vector<Light*> &lights)
    {
        // TODO: organize lights in a structure with efficient spatial search
        //  for now, just use a brute force technique by iterating over all registered lights.

        for(auto it = mLights.begin(); it != mLights.end(); ++it)
        {
            Light *l = *it;

            if(l->affects(point))
            {
                lights.push_back(l);
            }
        }
    }

    void RenderManager::getLightsIntersectingSphere(const osg::BoundingSphere &sphere, std::vector<Light*> &lights)
    {
        // TODO: organize lights in a structure with efficient spatial search
        //  for now, just use a brute force technique by iterating over all registered lights.

        for(auto it = mLights.begin(); it != mLights.end(); ++it)
        {
            Light *l = *it;

            if(l->affects(sphere))
            {
                lights.push_back(l);
            }
        }
    }

    void RenderManager::applyLayerLight(const osg::Matrix &viewMatrix, const osg::Vec3 &color, const osg::Vec3 &ambient, const osg::Vec3 &direction)
    {
        if(!mLightingEnabled)
        {
            return;
        }

        mLayerLightDiffuse->set(color);
        mLayerLightAmbient->set(ambient);

        osg::Vec4 dirCs = osg::Vec4(direction, 0.0) * viewMatrix;
        mLayerLightDirection->set(osg::Vec3(dirCs.x(), dirCs.y(), dirCs.z()));
    }

    void RenderManager::applyToLightUniform(const osg::Matrix &viewMatrix, Light *light, size_t index)
    {
        if(index >= OD_MAX_LIGHTS)
        {
            throw od::InvalidArgumentException("Tried to apply light at out-of-bounds index");
        }

        if(!mLightingEnabled)
        {
            return;
        }

        mLightDiffuseColors->setElement(index, light->getColor());
        mLightIntensities->setElement(index, light->getIntensityScaling());
        mLightRadii->setElement(index, light->getRadius());

        osg::Vec4 dirCs = osg::Vec4(light->getLevelObject()->getPosition(), 1.0) * viewMatrix;
        mLightPositions->setElement(index, osg::Vec3(dirCs.x(), dirCs.y(), dirCs.z()));
    }

    void RenderManager::applyNullLight(size_t index)
    {
        if(index >= OD_MAX_LIGHTS)
        {
            throw od::InvalidArgumentException("Tried to apply null light at out-of-bounds index");
        }

        if(!mLightingEnabled)
        {
            return;
        }

        mLightDiffuseColors->setElement(index, osg::Vec3(0.0, 0.0, 0.0));
        mLightIntensities->setElement(index, 0.0f);
    }

    void RenderManager::setEnableLighting(bool enable)
    {
        mLightingEnabled = enable;

        if(!enable)
        {
            mLayerLightAmbient->set(osg::Vec3(1.0, 1.0, 1.0));
            mLayerLightDiffuse->set(osg::Vec3(0.0, 0.0, 0.0));

            for(size_t i = 0; i < OD_MAX_LIGHTS; ++i)
            {
                mLightDiffuseColors->set(osg::Vec3(0.0, 0.0, 0.0));
            }
        }
    }

    void RenderManager::objectDeleted(void *object)
    {
        removeLight(static_cast<Light*>(object));
    }

}

