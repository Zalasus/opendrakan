/*
 * Renderer.cpp
 *
 *  Created on: 12 Nov 2018
 *      Author: zal
 */


#include <odOsg/Renderer.h>

#include <memory>

#include <odCore/Logger.h>
#include <odCore/LevelObject.h>

#include <odOsg/ObjectHandle.h>

namespace odOsg
{

    Renderer::Renderer()
    : mShaderFactory("resources/shader_src")
    {
        mViewer = new osgViewer::Viewer;

        mSceneRoot = new osg::Group;
        mSceneRoot->getOrCreateStateSet()->setAttribute(mShaderFactory.getProgram("default"));
        mViewer->setSceneData(mSceneRoot);

        mObjects = new osg::Group;
        mObjects->getOrCreateStateSet()->setAttribute(mShaderFactory.getProgram("model"));

        mLayers = new osg::Group;
        mLayers->getOrCreateStateSet()->setAttribute(mShaderFactory.getProgram("layer"));
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

    odRender::Handle *Renderer::createHandle(od::LevelObject &obj)
    {
        if(!obj.getClass()->hasModel())
        {
            return nullptr; // no need to create a handle for an object without a model
        }

        std::lock_guard<std::mutex> lock(mRenderMutex);

        std::unique_ptr<ObjectHandle> handle(new ObjectHandle(this, mObjects));
        handle->setPosition(obj.getPosition());
        handle->setOrientation(obj.getRotation());
        handle->setScale(obj.getScale());

        return handle.release();
    }

    odRender::Handle *Renderer::createHandle(od::Layer &layer)
    {
        return nullptr;
    }

    odRender::Geometry *Renderer::createGeometry()
    {
        return nullptr;
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
