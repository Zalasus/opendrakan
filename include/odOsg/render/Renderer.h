/*
 * Renderer.h
 *
 *  Created on: 12 Nov 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_RENDERER_H_
#define INCLUDE_ODOSG_RENDERER_H_

#include <vector>
#include <memory>

#include <osg/Group>
#include <osg/Uniform>
#include <osgViewer/Viewer>

#include <odCore/BoundingSphere.h>

#include <odCore/render/Renderer.h>

#include <odOsg/render/ShaderFactory.h>

namespace od
{
    class Light;
}

namespace odOsg
{
    class Texture;
    class Camera;
    class GuiNode;
    class Model;

    class Renderer : public odRender::Renderer
    {
    public:

        Renderer();
        ~Renderer();

        inline ShaderFactory &getShaderFactory() { return mShaderFactory; }
        inline osgViewer::Viewer *getViewer() { return mViewer; }

        virtual void setRendererEventListener(odRender::RendererEventListener *listener) override;

        virtual void setEnableLighting(bool b) override;
        virtual bool isLightingEnabled() const override;

        virtual std::shared_ptr<odRender::Handle> createHandle(odRender::RenderSpace space) override;
        virtual std::shared_ptr<odRender::Model> createModel() override;
        virtual std::shared_ptr<odRender::Geometry> createGeometry(odRender::PrimitiveType primitiveType, bool indexed) override;

        virtual std::shared_ptr<odRender::Model> createModelFromDb(std::shared_ptr<odDb::Model> model) override;
        virtual std::shared_ptr<odRender::Model> createModelFromLayer(od::Layer *layer) override;

        virtual std::shared_ptr<odRender::Image> createImageFromDb(std::shared_ptr<odDb::Texture> dbTexture) override;
        virtual std::shared_ptr<odRender::Texture> createTexture(std::shared_ptr<odRender::Image> image, odRender::TextureReuseSlot reuseSlot) override;
        virtual std::shared_ptr<odRender::GuiNode> createGuiNode() override;
        virtual std::shared_ptr<odRender::GuiNode> getGuiRootNode() override;

        virtual odRender::Camera *getCamera() override;

        virtual void setup() override;
        virtual void shutdown() override;

        virtual void frame(float relTime) override;

        void applyLayerLight(const osg::Matrix &viewMatrix, const osg::Vec3 &diffuse, const osg::Vec3 &ambient, const osg::Vec3 &direction);
        void applyToLightUniform(const osg::Matrix &viewMatrix, od::Light &light, size_t index);
        void applyNullLight(size_t index);

        void setFreeLook(bool f);


    private:

        void _setupGuiStuff();

        std::shared_ptr<Model> _buildSingleLodModelNode(odDb::Model &model);
        std::shared_ptr<Model> _buildMultiLodModelNode(odDb::Model &model);

        ShaderFactory mShaderFactory;

        odRender::RendererEventListener *mEventListener;

        std::shared_ptr<Camera> mCamera;

        bool mFreeLook;

        osg::ref_ptr<osgViewer::Viewer> mViewer;
        osg::ref_ptr<osg::Group> mSceneRoot;
        osg::ref_ptr<osg::Group> mLevelRoot;

        osg::ref_ptr<osg::Camera> mGuiCamera;
        osg::ref_ptr<osg::Group> mGuiRoot;
        std::shared_ptr<GuiNode> mGuiRootNode;

        bool mLightingEnabled;
        osg::ref_ptr<osg::Uniform> mGlobalLightDiffuse;
        osg::ref_ptr<osg::Uniform> mGlobalLightAmbient;
        osg::ref_ptr<osg::Uniform> mGlobalLightDirection;
        osg::ref_ptr<osg::Uniform> mLocalLightsColor;
        osg::ref_ptr<osg::Uniform> mLocalLightsRadius;
        osg::ref_ptr<osg::Uniform> mLocalLightsIntensity;
        osg::ref_ptr<osg::Uniform> mLocalLightsPosition;

        double mSimTime;
    };

}

#endif /* INCLUDE_ODOSG_RENDERER_H_ */
