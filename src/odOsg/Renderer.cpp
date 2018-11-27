/*
 * Renderer.cpp
 *
 *  Created on: 12 Nov 2018
 *      Author: zal
 */


#include <odOsg/Renderer.h>

#include <memory>

#include <osg/FrontFace>
#include <osgGA/TrackballManipulator>
#include <osgViewer/ViewerEventHandlers>

#include <odCore/Logger.h>
#include <odCore/LevelObject.h>
#include <odCore/OdDefines.h>

#include <odCore/render/Light.h>

#include <odOsg/ObjectNode.h>
#include <odOsg/ModelNode.h>
#include <odOsg/LayerNode.h>
#include <odOsg/Geometry.h>
#include <odOsg/Texture.h>
#include <odOsg/GlmAdapter.h>

namespace odOsg
{

    Renderer::Renderer()
    : mShaderFactory("resources/shader_src")
    , mLightingEnabled(false)
    {
        mViewer = new osgViewer::Viewer;

        osg::ref_ptr<osgViewer::StatsHandler> statsHandler(new osgViewer::StatsHandler);
        statsHandler->setKeyEventPrintsOutStats(0);
        statsHandler->setKeyEventTogglesOnScreenStats(osgGA::GUIEventAdapter::KEY_F1);
        mViewer->addEventHandler(statsHandler);

        mViewer->setCameraManipulator(new osgGA::TrackballManipulator, true);
        mViewer->setKeyEventSetsDone(osgGA::GUIEventAdapter::KEY_Escape);

        mSceneRoot = new osg::Group;
        mViewer->setSceneData(mSceneRoot);

        mObjects = new osg::Group;
        mObjects->getOrCreateStateSet()->setAttribute(new osg::FrontFace(osg::FrontFace::CLOCKWISE));
        mSceneRoot->addChild(mObjects);

        mLayers = new osg::Group;
        mSceneRoot->addChild(mLayers);

        // set up root state
        osg::StateSet *ss = mSceneRoot->getOrCreateStateSet();
        ss->setAttribute(mShaderFactory.getProgram("default"));
        ss->setMode(GL_CULL_FACE, osg::StateAttribute::ON);

        mGlobalLightDiffuse   = new osg::Uniform("layerLightDiffuse",   osg::Vec3(0.0, 0.0, 0.0));
        mGlobalLightAmbient   = new osg::Uniform("layerLightAmbient",   osg::Vec3(1.0, 1.0, 1.0));
        mGlobalLightDirection = new osg::Uniform("layerLightDirection", osg::Vec3(0.0, 1.0, 0.0));
        ss->addUniform(mGlobalLightDiffuse);
        ss->addUniform(mGlobalLightAmbient);
        ss->addUniform(mGlobalLightDirection);

        mLocalLightsColor     = new osg::Uniform(osg::Uniform::FLOAT_VEC3, "objectLightDiffuse", OD_MAX_LIGHTS);
        mLocalLightsIntensity = new osg::Uniform(osg::Uniform::FLOAT, "objectLightIntensity", OD_MAX_LIGHTS);
        mLocalLightsRadius    = new osg::Uniform(osg::Uniform::FLOAT, "objectLightRadius", OD_MAX_LIGHTS);
        mLocalLightsPosition  = new osg::Uniform(osg::Uniform::FLOAT_VEC3, "objectLightPosition", OD_MAX_LIGHTS);
        ss->addUniform(mLocalLightsColor);
        ss->addUniform(mLocalLightsIntensity);
        ss->addUniform(mLocalLightsRadius);
        ss->addUniform(mLocalLightsPosition);
    }

    Renderer::~Renderer()
    {
        if(mRenderThread.joinable() && mViewer != nullptr)
        {
            Logger::warn() << "Render thread was not stopped when renderer was destroyed";
            mViewer->setDone(true);
            mRenderThread.join();
        }
    }

    void Renderer::onStart()
    {
        mRenderThread = std::thread(&Renderer::_threadedRender, this);
    }

    void Renderer::onEnd()
    {
        if(mRenderThread.joinable() && mViewer != nullptr)
        {
            mViewer->setDone(true);
            mRenderThread.join();
        }
    }

    void Renderer::setEnableLighting(bool b)
    {
        mLightingEnabled = b;
    }

    bool Renderer::isLightingEnabled() const
    {
        return mLightingEnabled;
    }

    void Renderer::addLight(odRender::Light *light)
    {
        mLights.push_back(light);
    }

    odRender::ObjectNode *Renderer::createObjectNode(od::LevelObject &obj)
    {
        return new ObjectNode(this, mObjects);
    }

    odRender::ModelNode *Renderer::createModelNode(odDb::Model *model)
    {
        return new ModelNode(this, model);
    }

    odRender::LayerNode *Renderer::createLayerNode(od::Layer *layer)
    {
        return new LayerNode(this, layer, mLayers);
    }

    odRender::Texture *Renderer::createTexture(odDb::Texture *texture)
    {
        return new Texture(this, texture);
    }

    void Renderer::applyLayerLight(const osg::Matrix &viewMatrix, const osg::Vec3 &diffuse, const osg::Vec3 &ambient, const osg::Vec3 &direction)
    {
        if(!mLightingEnabled)
        {
            return;
        }

        mGlobalLightDiffuse->set(diffuse);
        mGlobalLightAmbient->set(ambient);

        osg::Vec4 dirCs = osg::Vec4(direction, 0.0) * viewMatrix;
        mGlobalLightDirection->set(osg::Vec3(dirCs.x(), dirCs.y(), dirCs.z()));
    }

    void Renderer::applyToLightUniform(const osg::Matrix &viewMatrix, odRender::Light *light, size_t index)
    {
        if(index >= OD_MAX_LIGHTS)
        {
            throw od::InvalidArgumentException("Tried to apply light at out-of-bounds index");
        }

        if(!mLightingEnabled)
        {
            return;
        }

        mLocalLightsColor->setElement(index, GlmAdapter::toOsg(light->getColor()));
        mLocalLightsIntensity->setElement(index, light->getIntensityScaling());
        mLocalLightsRadius->setElement(index, light->getRadius());

        osg::Vec3 posWs = GlmAdapter::toOsg(light->getLevelObject()->getPosition());
        osg::Vec4 dirCs = osg::Vec4(posWs, 1.0) * viewMatrix;
        mLocalLightsPosition->setElement(index, osg::Vec3(dirCs.x(), dirCs.y(), dirCs.z()));
    }

    void Renderer::applyNullLight(size_t index)
    {
        if(index >= OD_MAX_LIGHTS)
        {
            throw od::InvalidArgumentException("Tried to apply null light at out-of-bounds index");
        }

        if(!mLightingEnabled)
        {
            return;
        }

        mLocalLightsColor->setElement(index, osg::Vec3(0.0, 0.0, 0.0));
        mLocalLightsIntensity->setElement(index, 0.0f);
    }

    void Renderer::getLightsIntersectingSphere(const od::BoundingSphere &sphere, std::vector<odRender::Light*> &lights)
    {
        // TODO: organize lights in a structure with efficient spatial search
        //  for now, just use a brute force technique by iterating over all registered lights.

        for(auto it = mLights.begin(); it != mLights.end(); ++it)
        {
            odRender::Light *l = *it;

            if(l->affects(sphere))
            {
                lights.push_back(l);
            }
        }
    }

    void Renderer::_threadedRender()
    {
        static const double maxFrameRate = 60;

        mViewer->realize();

        double simTime = 0;
        double frameTime = 0;
        while(!mViewer->done())
        {
            double minFrameTime = (maxFrameRate > 0.0) ? (1.0/maxFrameRate) : 0.0;
            osg::Timer_t startFrameTick = osg::Timer::instance()->tick();

            {
                std::lock_guard<std::mutex> lock(mRenderMutex);

                mViewer->advance(simTime);
                mViewer->eventTraversal();
                mViewer->updateTraversal();
                mViewer->renderingTraversals();
            }

            osg::Timer_t endFrameTick = osg::Timer::instance()->tick();
            frameTime = osg::Timer::instance()->delta_s(startFrameTick, endFrameTick);
            simTime += frameTime;
            if(frameTime < minFrameTime)
            {
                simTime += (minFrameTime-frameTime);
                std::this_thread::sleep_for(std::chrono::microseconds(1000000*static_cast<size_t>(minFrameTime-frameTime)));
            }
        }

        // FIXME: we need to notify the engine that it should shut down here
    }

}
