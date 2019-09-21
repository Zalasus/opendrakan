/*
 * Renderer.h
 *
 *  Created on: Nov 10, 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_RENDERER_H_
#define INCLUDE_ODCORE_RENDER_RENDERER_H_

#include <odCore/RefCounted.h>

#include <odCore/render/Geometry.h>

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
    class Texture;
    class Image;
    class RendererEventListener;
    class GuiNode;
    class Handle;
    class Model;

    enum class RenderSpace
    {
        NONE,
        LEVEL,
        GUI
    };

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

        virtual od::RefPtr<Handle> createHandle(RenderSpace space) = 0;
        virtual od::RefPtr<Model> createModel() = 0;
        virtual od::RefPtr<Geometry> createGeometry(PrimitiveType primitiveType, bool indexed) = 0;

        virtual od::RefPtr<Model> createModelFromDb(odDb::Model *model) = 0;
        virtual od::RefPtr<Model> createModelFromLayer(od::Layer *layer) = 0;

        virtual od::RefPtr<Image> createImage(odDb::Texture *dbTexture) = 0;
        virtual od::RefPtr<Texture> createTexture(Image *image) = 0;
        virtual od::RefPtr<GuiNode> createGuiNode(odGui::Widget *widget) = 0;
        virtual GuiNode *getGuiRootNode() = 0;

        virtual Camera *getCamera() = 0;

        /**
         * @brief Notifies the renderer that a logic tick has happened.
         *
         * Since the tick rate is not guaranteed to be fixed, the parameter relTime should be used
         * to correctly interpolate between ticks.
         *
         * @param relTime  Time since last tick, in seconds.
         */
        virtual void advance(float relTime) = 0;

        od::RefPtr<Handle> createHandleFromObject(od::LevelObject &obj);

    };

}


#endif /* INCLUDE_ODCORE_RENDER_RENDERER_H_ */
