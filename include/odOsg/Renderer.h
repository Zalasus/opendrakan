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

#include <osg/Group>
#include <osg/Uniform>
#include <osgViewer/Viewer>

#include <odCore/RefCounted.h>
#include <odCore/BoundingSphere.h>

#include <odOsg/ShaderFactory.h>

namespace odOsg
{

    class ModelNode;
    class Texture;
    class Camera;

    class Renderer : public odRender::Renderer
    {
    public:

        Renderer();
        ~Renderer();

        inline ShaderFactory &getShaderFactory() { return mShaderFactory; }

        virtual void onStart() override;
        virtual void onEnd() override;
        virtual void setRendererEventListener(odRender::RendererEventListener *listener) override;

        virtual void setEnableLighting(bool b) override;
        virtual bool isLightingEnabled() const override;
        virtual odRender::Light *createLight(od::LevelObject *obj) override;

        virtual odRender::ObjectNode *createObjectNode(od::LevelObject &obj) override;
        virtual odRender::ModelNode *createModelNode(odDb::Model *model) override;
        virtual odRender::LayerNode *createLayerNode(od::Layer *layer) override;
        virtual odRender::Image *createImage(odDb::Texture *dbTexture) override;
        virtual odRender::Texture *createTexture(odRender::Image *image) override;

        virtual odRender::Camera *getCamera() override;

        void applyLayerLight(const osg::Matrix &viewMatrix, const osg::Vec3 &diffuse, const osg::Vec3 &ambient, const osg::Vec3 &direction);
        void applyToLightUniform(const osg::Matrix &viewMatrix, odRender::Light *light, size_t index);
        void applyNullLight(size_t index);
        void getLightsIntersectingSphere(const od::BoundingSphere &sphere, std::vector<odRender::Light*> &lights);

        void setFreeLook(bool f);


    private:

        void _threadedRender();

        ShaderFactory mShaderFactory;
        std::thread mRenderThread;
        std::mutex mRenderMutex;

        odRender::RendererEventListener *mEventListener;

        od::RefPtr<Camera> mCamera;

        osg::ref_ptr<osgViewer::Viewer> mViewer;
        osg::ref_ptr<osg::Group> mSceneRoot;
        osg::ref_ptr<osg::Group> mObjects;
        osg::ref_ptr<osg::Group> mLayers;

        bool mLightingEnabled;
        std::vector<od::RefPtr<odRender::Light>> mLights;
        osg::ref_ptr<osg::Uniform> mGlobalLightDiffuse;
        osg::ref_ptr<osg::Uniform> mGlobalLightAmbient;
        osg::ref_ptr<osg::Uniform> mGlobalLightDirection;
        osg::ref_ptr<osg::Uniform> mLocalLightsColor;
        osg::ref_ptr<osg::Uniform> mLocalLightsRadius;
        osg::ref_ptr<osg::Uniform> mLocalLightsIntensity;
        osg::ref_ptr<osg::Uniform> mLocalLightsPosition;
    };

}

#endif /* INCLUDE_ODOSG_RENDERER_H_ */
