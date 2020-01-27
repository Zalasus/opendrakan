/**
 * @file GlRenderer.h
 * @date Jan 26, 2020
 * @author zal
 */

#ifndef INCLUDE_ODGL_GLRENDERER_H_
#define INCLUDE_ODGL_GLRENDERER_H_

#include <vector>
#include <memory>

#include <odGl/glad.h>
#include <GLFW/glfw3.h>

#include <odCore/RefCounted.h>
#include <odCore/BoundingSphere.h>

#include <odCore/render/Renderer.h>

namespace od
{
    class Light;
}

namespace odGl
{

    class GlModel;

    class GlRenderer : public odRender::Renderer
    {
    public:

        GlRenderer();
        ~GlRenderer();

        virtual void setRendererEventListener(odRender::RendererEventListener *listener) override;

        virtual void setEnableLighting(bool b) override;
        virtual bool isLightingEnabled() const override;

        virtual od::RefPtr<odRender::Handle> createHandle(odRender::RenderSpace space) override;
        virtual od::RefPtr<odRender::Model> createModel() override;
        virtual od::RefPtr<odRender::Geometry> createGeometry(odRender::PrimitiveType primitiveType, bool indexed) override;

        virtual od::RefPtr<odRender::Model> createModelFromDb(odDb::Model *model) override;
        virtual od::RefPtr<odRender::Model> createModelFromLayer(od::Layer *layer) override;

        virtual od::RefPtr<odRender::Image> createImage(odDb::Texture *dbTexture) override;
        virtual od::RefPtr<odRender::Texture> createTexture(odRender::Image *image) override;
        virtual od::RefPtr<odRender::GuiNode> createGuiNode(odGui::Widget *widget) override;
        virtual odRender::GuiNode *getGuiRootNode() override;

        virtual odRender::Camera *getCamera() override;

        virtual void setup() override;
        virtual void shutdown() override;

        virtual void frame(float relTime) override;

        void setFreeLook(bool f);


    private:

        void _setupGuiStuff();

        od::RefPtr<GlModel> _buildSingleLodModelNode(odDb::Model *model);
        od::RefPtr<GlModel> _buildMultiLodModelNode(odDb::Model *model);


        GLFWwindow *mWindow;

    };

}


#endif /* INCLUDE_ODGL_GLRENDERER_H_ */
