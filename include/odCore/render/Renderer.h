/*
 * Renderer.h
 *
 *  Created on: Nov 10, 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_RENDERER_H_
#define INCLUDE_ODCORE_RENDER_RENDERER_H_

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

namespace odRender
{

    class Camera;
    class Texture;
    class Image;
    class RendererEventListener;
    class Handle;
    class Model;
    class GuiCallback;

    /**
     * @brief Used when creating a handle. Determines where and how that handle is to be rendered.
     *
     * GUI render space uses orthogonal projection, depth-sorted drawing order and a simplified coordinate
     * system where the top-left of the screen is 0/0 and the bottom right is 1/1.
     */
    enum class RenderSpace
    {
        NONE, ///< Handle will not be rendered
        LEVEL, ///< Handle will be rendered normally in the level relative to the level origin
        GUI ///< Handle will be rendered as part of the HUD, with enforced depth sorting and orthogonal projection
    };

    enum class TextureReuseSlot
    {
        NONE,
        OBJECT,
        LAYER
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

        virtual std::shared_ptr<Handle> createHandle(RenderSpace space) = 0;
        virtual std::shared_ptr<Model> createModel() = 0;
        virtual std::shared_ptr<Geometry> createGeometry(PrimitiveType primitiveType, bool indexed) = 0;

        virtual std::shared_ptr<Model> createModelFromDb(std::shared_ptr<odDb::Model> model) = 0;

        /**
         * @brief Uses createModelFromDb() to create a new Model and caches it in model. Once the cache exists, no further calls to createModelFromDb() are needed.
         */
        std::shared_ptr<Model> getOrCreateModelFromDb(std::shared_ptr<odDb::Model> model);

        virtual std::shared_ptr<Model> createModelFromLayer(od::Layer *layer) = 0;

        virtual std::shared_ptr<Image> createImageFromDb(std::shared_ptr<odDb::Texture> dbTexture) = 0;

        /**
         * @brief Uses createImageFromDb() to create an Image and caches it in dbTexture. Once the cache exists, no further calls to createImageFromDb() are needed.
         */
        std::shared_ptr<Image> getOrCreateImageFromDb(std::shared_ptr<odDb::Texture> dbTexture);

        virtual std::shared_ptr<Texture> createTexture(std::shared_ptr<Image> image, TextureReuseSlot reuseSlot) = 0;

        virtual void addGuiCallback(GuiCallback *callback) = 0;
        virtual void removeGuiCallback(GuiCallback *callback) = 0;
        virtual glm::vec2 getFramebufferDimensions() = 0;

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

        std::shared_ptr<Handle> createHandleFromObject(od::LevelObject &obj);

    };

}


#endif /* INCLUDE_ODCORE_RENDER_RENDERER_H_ */
