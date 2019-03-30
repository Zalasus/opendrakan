/*
 * Renderer.cpp
 *
 *  Created on: 12 Nov 2018
 *      Author: zal
 */


#include <odCore/Downcast.h>
#include <odOsg/render/Renderer.h>

#include <osg/FrontFace>
#include <osgGA/TrackballManipulator>
#include <osgViewer/ViewerEventHandlers>

#include <odCore/Logger.h>
#include <odCore/LevelObject.h>
#include <odCore/Light.h>
#include <odCore/render/RendererEventListener.h>

#include <odOsg/GlmAdapter.h>
#include <odOsg/Constants.h>

#include <odOsg/render/ObjectNode.h>
#include <odOsg/render/ModelNode.h>
#include <odOsg/render/LayerNode.h>
#include <odOsg/render/Geometry.h>
#include <odOsg/render/Image.h>
#include <odOsg/render/Texture.h>
#include <odOsg/render/Camera.h>
#include <odOsg/render/GuiNode.h>

namespace odOsg
{

    Renderer::Renderer()
    : mShaderFactory("resources/shader_src")
    , mEventListener(nullptr)
    , mFreeLook(false)
    , mLightingEnabled(true)
    {
        mViewer = new osgViewer::Viewer;

        mCamera = od::make_refd<Camera>(mViewer->getCamera());

        osg::ref_ptr<osgViewer::StatsHandler> statsHandler(new osgViewer::StatsHandler);
        statsHandler->setKeyEventPrintsOutStats(0);
        statsHandler->setKeyEventTogglesOnScreenStats(osgGA::GUIEventAdapter::KEY_F1);
        mViewer->addEventHandler(statsHandler);

        mViewer->setKeyEventSetsDone(osgGA::GUIEventAdapter::KEY_Escape);

        mSceneRoot = new osg::Group;
        mViewer->setSceneData(mSceneRoot);

        // set up root state
        osg::StateSet *ss = mSceneRoot->getOrCreateStateSet();
        ss->setMode(GL_CULL_FACE, osg::StateAttribute::ON);

        ss->setDefine("MAX_LIGHTS", std::to_string(Constants::MAX_LIGHTS));
        ss->setDefine("MAX_BONES", std::to_string(Constants::MAX_BONES));

        mGlobalLightDiffuse   = new osg::Uniform("layerLightDiffuse",   osg::Vec3(0.0, 0.0, 0.0));
        mGlobalLightAmbient   = new osg::Uniform("layerLightAmbient",   osg::Vec3(0.0, 0.0, 0.0));
        mGlobalLightDirection = new osg::Uniform("layerLightDirection", osg::Vec3(0.0, 1.0, 0.0));
        ss->addUniform(mGlobalLightDiffuse);
        ss->addUniform(mGlobalLightAmbient);
        ss->addUniform(mGlobalLightDirection);

        mLocalLightsColor     = new osg::Uniform(osg::Uniform::FLOAT_VEC3, "objectLightDiffuse", Constants::MAX_LIGHTS);
        mLocalLightsIntensity = new osg::Uniform(osg::Uniform::FLOAT, "objectLightIntensity", Constants::MAX_LIGHTS);
        mLocalLightsRadius    = new osg::Uniform(osg::Uniform::FLOAT, "objectLightRadius", Constants::MAX_LIGHTS);
        mLocalLightsPosition  = new osg::Uniform(osg::Uniform::FLOAT_VEC3, "objectLightPosition", Constants::MAX_LIGHTS);
        ss->addUniform(mLocalLightsColor);
        ss->addUniform(mLocalLightsIntensity);
        ss->addUniform(mLocalLightsRadius);
        ss->addUniform(mLocalLightsPosition);

        ss->setAttribute(mShaderFactory.getProgram("default"), osg::StateAttribute::ON);

        mLayers = new osg::Group;
        osg::ref_ptr<osg::Program> layerProg = getShaderFactory().getProgram("layer");
        mLayers->getOrCreateStateSet()->setAttribute(layerProg, osg::StateAttribute::ON);
        mSceneRoot->addChild(mLayers);

        mObjects = new osg::Group;
        mObjects->getOrCreateStateSet()->setAttribute(new osg::FrontFace(osg::FrontFace::CLOCKWISE));
        mSceneRoot->addChild(mObjects);

        _setupGuiStuff();
    }

    Renderer::~Renderer()
    {
        if(mViewer != nullptr)
        {
            Logger::warn() << "Render thread was not stopped when renderer was destroyed";
            mViewer->setDone(true);
        }

        // note: we need to do this even if the render thread already left it's thread function, or else it will std::terminate() us
        if(mRenderThread.joinable())
        {
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

    void Renderer::setRendererEventListener(odRender::RendererEventListener *listener)
    {
        mEventListener = listener;
    }

    void Renderer::setEnableLighting(bool b)
    {
        mLightingEnabled = b;

        if(!b)
        {
            mGlobalLightAmbient->set(osg::Vec3(1.0, 1.0, 1.0));
        }
    }

    bool Renderer::isLightingEnabled() const
    {
        return mLightingEnabled;
    }

    od::RefPtr<odRender::ObjectNode> Renderer::createObjectNode(od::LevelObject &obj)
    {
        auto on = od::make_refd<ObjectNode>(this, mObjects);
        return od::RefPtr<odRender::ObjectNode>(on);
    }

    od::RefPtr<odRender::ModelNode> Renderer::createModelNode(odDb::Model *model)
    {
        auto mn = od::make_refd<ModelNode>(this, model);
        return od::RefPtr<odRender::ModelNode>(mn);
    }

    od::RefPtr<odRender::LayerNode> Renderer::createLayerNode(od::Layer *layer)
    {
        auto ln = od::make_refd<LayerNode>(this, layer, mLayers);
        return od::RefPtr<odRender::LayerNode>(ln);
    }

    od::RefPtr<odRender::Image> Renderer::createImage(odDb::Texture *dbTexture)
    {
        auto image = od::make_refd<Image>(dbTexture);
        return od::RefPtr<odRender::Image>(image);
    }

    od::RefPtr<odRender::Texture> Renderer::createTexture(odRender::Image *image)
    {
        Image *odOsgImage = od::confident_downcast<Image>(image);
        auto texture = od::make_refd<Texture>(odOsgImage);
        return od::RefPtr<odRender::Texture>(texture);
    }

    od::RefPtr<odRender::GuiNode> Renderer::createGuiNode(odGui::Widget *widget)
    {
        auto gn = od::make_refd<GuiNode>(this, widget);
        return od::RefPtr<odRender::GuiNode>(gn);
    }

    odRender::GuiNode *Renderer::getGuiRootNode()
    {
        return mGuiRootNode;
    }

    odRender::Camera *Renderer::getCamera()
    {
        return mCamera;
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

    void Renderer::applyToLightUniform(const osg::Matrix &viewMatrix, od::Light *light, size_t index)
    {
        if(index >= mLocalLightsColor->getNumElements())
        {
            throw od::InvalidArgumentException("Tried to apply light at out-of-bounds index");
        }

        if(light == nullptr)
        {
            throw od::InvalidArgumentException("Passed nullptr light to renderer");
        }

        if(!mLightingEnabled)
        {
            return;
        }

        mLocalLightsColor->setElement(index, GlmAdapter::toOsg(light->getColor()));
        mLocalLightsIntensity->setElement(index, light->getIntensityScaling());
        mLocalLightsRadius->setElement(index, light->getRadius());

        osg::Vec3 posWs = GlmAdapter::toOsg(light->getPosition());
        osg::Vec4 dirCs = osg::Vec4(posWs, 1.0) * viewMatrix;
        mLocalLightsPosition->setElement(index, osg::Vec3(dirCs.x(), dirCs.y(), dirCs.z()));
    }

    void Renderer::applyNullLight(size_t index)
    {
        if(index >= mLocalLightsColor->getNumElements())
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

    void Renderer::setFreeLook(bool f)
    {
        mFreeLook = f;

        mCamera->setIgnoreViewChanges(f);

        if(f)
        {
            mViewer->setCameraManipulator(new osgGA::TrackballManipulator, true);

        }else
        {
            mViewer->setCameraManipulator(nullptr, false);
        }
    }

    void Renderer::_setupGuiStuff()
    {
        mGuiCamera = new osg::Camera;
        mGuiCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
        mGuiCamera->setProjectionMatrix(osg::Matrix::ortho2D(-1, 1, -1, 1));
        mGuiCamera->setViewMatrix(osg::Matrix::identity());
        mGuiCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
        mGuiCamera->setRenderOrder(osg::Camera::POST_RENDER);
        mGuiCamera->setAllowEventFocus(false);
        mSceneRoot->addChild(mGuiCamera);

        mGuiRoot = new osg::Group;
        mGuiRoot->setCullingActive(false);
        osg::StateSet *ss = mGuiRoot->getOrCreateStateSet();
        ss->setMode(GL_BLEND, osg::StateAttribute::ON);
        ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
        ss->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);
        mGuiCamera->addChild(mGuiRoot);

        mGuiRootNode = od::make_refd<GuiNode>(this, nullptr);
        mGuiRoot->addChild(mGuiRootNode->getOsgNode());
    }

    void Renderer::_threadedRender()
    {
        static const double maxFrameRate = 60;

        mViewer->realize();

        osgViewer::Viewer::Windows windows;
        mViewer->getWindows(windows, true);
        for(osgViewer::GraphicsWindow *window: windows)
        {
            window->setWindowName("OpenDrakan (OSG)");

            if(!mFreeLook)
            {
                window->setCursor(osgViewer::GraphicsWindow::NoCursor);
            }
        }

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

        mViewer = nullptr;

        if(mEventListener != nullptr)
        {
            mEventListener->onRenderWindowClosed();
        }

        Logger::verbose() << "Render thread terminated";
    }

}
