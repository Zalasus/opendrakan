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
     * @brief Interface for a renderer implementation.
     */
    class Renderer
    {
    public:

        virtual ~Renderer() = default;

        inline void toggleLighting() { setEnableLighting(!isLightingEnabled()); }

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
         * @brief Called once right before rendering starts.
         *
         * Use this to start subthreads, preallocate stuff etc. if needed.
         */
        virtual void setup() = 0;

        /**
         * @brief Called once after rendering stops.
         *
         * Stop your subthreads here, if needed.
         */
        virtual void shutdown() = 0;

        /**
         * @brief Renders a frame.
         * @param relTime  Time passed since last frame was rendered, in seconds.
         */
        virtual void frame(float relTime) = 0;

        od::RefPtr<Handle> createHandleFromObject(od::LevelObject &obj);

    };

}


#endif /* INCLUDE_ODCORE_RENDER_RENDERER_H_ */
