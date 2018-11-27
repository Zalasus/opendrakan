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

#include <odOsg/ObjectNode.h>
#include <odOsg/ModelNode.h>
#include <odOsg/LayerNode.h>
#include <odOsg/Geometry.h>
#include <odOsg/Texture.h>

namespace odOsg
{

    Renderer::Renderer()
    : mShaderFactory("resources/shader_src")
    {
        mViewer = new osgViewer::Viewer;

        osg::ref_ptr<osgViewer::StatsHandler> statsHandler(new osgViewer::StatsHandler);
        statsHandler->setKeyEventPrintsOutStats(0);
        statsHandler->setKeyEventTogglesOnScreenStats(osgGA::GUIEventAdapter::KEY_F1);
        mViewer->addEventHandler(statsHandler);

        mViewer->setCameraManipulator(new osgGA::TrackballManipulator, true);
        mViewer->setKeyEventSetsDone(osgGA::GUIEventAdapter::KEY_Escape);

        mSceneRoot = new osg::Group;
        mSceneRoot->getOrCreateStateSet()->setAttribute(mShaderFactory.getProgram("default"));
        mSceneRoot->getOrCreateStateSet()->setMode(GL_CULL_FACE, osg::StateAttribute::ON);
        mViewer->setSceneData(mSceneRoot);

        mObjects = new osg::Group;
        mObjects->getOrCreateStateSet()->setAttribute(new osg::FrontFace(osg::FrontFace::CLOCKWISE));
        mSceneRoot->addChild(mObjects);

        mLayers = new osg::Group;
        mSceneRoot->addChild(mLayers);

        mGlobalAmbient = new osg::Uniform("layerLightAmbient", osg::Vec3(1.0, 1.0, 1.0));
        mSceneRoot->getOrCreateStateSet()->addUniform(mGlobalAmbient);
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
    }

    bool Renderer::isLightingEnabled() const
    {
        return false;
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
