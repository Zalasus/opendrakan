/*
 * Renderer.h
 *
 *  Created on: 12 Nov 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_RENDERER_H_
#define INCLUDE_ODOSG_RENDERER_H_

#include <odCore/render/Renderer.h>

#include <thread>
#include <mutex>
#include <vector>
#include <memory>

#include <osg/Group>
#include <osg/Uniform>
#include <osgViewer/Viewer>

#include <odCore/RefCounted.h>
#include <odCore/BoundingSphere.h>

#include <odOsg/render/ShaderFactory.h>

namespace od
{
    class Light;
}

namespace odOsg
{
    class ModelNode;
    class Texture;
    class Camera;
    class GuiNode;
    class PhysicsDebugDrawer;

    class Renderer : public odRender::Renderer
    {
    public:

        Renderer();
        ~Renderer();

        inline ShaderFactory &getShaderFactory() { return mShaderFactory; }
        inline osgViewer::Viewer *getViewer() { return mViewer; }

        virtual void onStart() override;
        virtual void onEnd() override;
        virtual void setRendererEventListener(odRender::RendererEventListener *listener) override;

        virtual void setEnableLighting(bool b) override;
        virtual bool isLightingEnabled() const override;

        virtual od::RefPtr<odRender::ObjectNode> createObjectNode(od::LevelObject &obj) override;
        virtual od::RefPtr<odRender::ModelNode> createModelNode(odDb::Model *model) override;
        virtual od::RefPtr<odRender::LayerNode> createLayerNode(od::Layer *layer) override;
        virtual od::RefPtr<odRender::Image> createImage(odDb::Texture *dbTexture) override;
        virtual od::RefPtr<odRender::Texture> createTexture(odRender::Image *image) override;
        virtual od::RefPtr<odRender::GuiNode> createGuiNode(odGui::Widget *widget) override;
        virtual odRender::GuiNode *getGuiRootNode() override;

        virtual odRender::Camera *getCamera() override;

        virtual odRender::PhysicsDebugDrawer *getPhysicsDebugDrawer() override;

        void applyLayerLight(const osg::Matrix &viewMatrix, const osg::Vec3 &diffuse, const osg::Vec3 &ambient, const osg::Vec3 &direction);
        void applyToLightUniform(const osg::Matrix &viewMatrix, od::Light *light, size_t index);
        void applyNullLight(size_t index);

        void setFreeLook(bool f);


    private:

        void _setupGuiStuff();

        void _threadedRender();

        ShaderFactory mShaderFactory;
        std::thread mRenderThread;
        std::mutex mRenderMutex;

        odRender::RendererEventListener *mEventListener;

        od::RefPtr<Camera> mCamera;

        bool mFreeLook;

        osg::ref_ptr<osgViewer::Viewer> mViewer;
        osg::ref_ptr<osg::Group> mSceneRoot;
        osg::ref_ptr<osg::Group> mObjects;
        osg::ref_ptr<osg::Group> mLayers;

        osg::ref_ptr<osg::Camera> mGuiCamera;
        osg::ref_ptr<osg::Group> mGuiRoot;
        od::RefPtr<GuiNode> mGuiRootNode;

        bool mLightingEnabled;
        osg::ref_ptr<osg::Uniform> mGlobalLightDiffuse;
        osg::ref_ptr<osg::Uniform> mGlobalLightAmbient;
        osg::ref_ptr<osg::Uniform> mGlobalLightDirection;
        osg::ref_ptr<osg::Uniform> mLocalLightsColor;
        osg::ref_ptr<osg::Uniform> mLocalLightsRadius;
        osg::ref_ptr<osg::Uniform> mLocalLightsIntensity;
        osg::ref_ptr<osg::Uniform> mLocalLightsPosition;

        std::unique_ptr<PhysicsDebugDrawer> mPhysicsDebugDrawer;
    };

}

#endif /* INCLUDE_ODOSG_RENDERER_H_ */
