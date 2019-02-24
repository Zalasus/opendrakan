/*
 * Renderer.h
 *
 *  Created on: Nov 10, 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_RENDERER_H_
#define INCLUDE_ODCORE_RENDER_RENDERER_H_

#include <odCore/RefCounted.h>

namespace od
{
    class LevelObject;
    class Layer;
}

namespace odDb
{
    class Model;
    class Texture;
}

namespace odGui
{
    class Widget;
}

namespace odRender
{

    class Camera;
    class ObjectNode;
    class ModelNode;
    class LayerNode;
    class Texture;
    class Image;
    class RendererEventListener;
    class GuiNode;
    class PhysicsDebugDrawer;

    /**
     * Interface for a renderer implementation.
     */
    class Renderer
    {
    public:

        virtual ~Renderer() = default;

        inline void toggleLighting() { setEnableLighting(!isLightingEnabled()); }

        virtual void onStart() = 0;
        virtual void onEnd() = 0;
        virtual void setRendererEventListener(RendererEventListener *listener) = 0;

        virtual void setEnableLighting(bool b) = 0;
        virtual bool isLightingEnabled() const = 0;

        virtual od::RefPtr<ObjectNode> createObjectNode(od::LevelObject &obj) = 0;
        virtual od::RefPtr<ModelNode> createModelNode(odDb::Model *model) = 0;
        virtual od::RefPtr<LayerNode> createLayerNode(od::Layer *layer) = 0;
        virtual od::RefPtr<Image> createImage(odDb::Texture *dbTexture) = 0;
        virtual od::RefPtr<Texture> createTexture(Image *image) = 0;
        virtual od::RefPtr<GuiNode> createGuiNode(odGui::Widget *widget) = 0;
        virtual GuiNode *getGuiRootNode() = 0;

        virtual Camera *getCamera() = 0;

        virtual PhysicsDebugDrawer *getPhysicsDebugDrawer() = 0;

    };

}


#endif /* INCLUDE_ODCORE_RENDER_RENDERER_H_ */
