
#ifndef INCLUDE_GUI_QUAD_H_
#define INCLUDE_GUI_QUAD_H_

#include <memory>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <odCore/db/Asset.h>

namespace odRender
{
    class Handle;
    class Renderer;
    class Model;
    class Geometry;
    class Texture;
}

namespace odGui
{

    /**
     * @brief A screen-space textured quad for GUI elements.
     *
     * This is a convenience wrapper around the common renderer-created objects.
     * It provides you with a render handle that can be added to your widgets,
     * as well as some operations on the quad.
     *
     * This is sorta intended to be used in a composition context where you just
     * want to add a bunch of Quads to your widget without managing each and
     * every handle, texture etc. But since sometimes you want to create Quads
     * on-demand, a Quad can be empty. This helps so that you don't have to
     * heap-allocate this and can instead assign to it or use init() when you
     * need it.
     *
     * Since all renderer objects are shared, you don't have to keep Quad
     * objects around forever. You can use this as a sort of builder for a quad
     * render handle and let it die once you've passed the finished handle to
     * the widget.
     *
     * Since basically any Quad in a GUI tends to be transparent in some way,
     * the handle is put into the transparency bin by default. If you do not
     * want that, you have to change it yourself.
     */
    class Quad
    {
    public:

        Quad();
        Quad(Quad &&q) = default;
        Quad(odRender::Renderer &renderer);

        inline std::shared_ptr<odRender::Handle> getHandle() { return mHandle; }
        inline std::shared_ptr<odRender::Model> getModel() { return mModel; }
        inline std::shared_ptr<odRender::Geometry> getGeometry() { return mGeometry; }
        inline std::shared_ptr<odRender::Texture> getTexture() { return mTexture; }
        inline bool empty() const { return mHandle == nullptr; }

        void init(odRender::Renderer &renderer);

        void setTexture(std::shared_ptr<odRender::Texture> texture);

        void setTextureCoords(const glm::vec2 &topLeft, const glm::vec2 &bottomRight);
        void setTextureCoordsFromPixels(const glm::vec2 &topLeft, const glm::vec2 &bottomRight);
        void setVertexCoords(const glm::vec2 &topLeft, const glm::vec2 &bottomRight);

        void setColor(const glm::vec4 &color);


    private:

        void _check();

        std::shared_ptr<odRender::Handle> mHandle;
        std::shared_ptr<odRender::Model> mModel;
        std::shared_ptr<odRender::Geometry> mGeometry;
        std::shared_ptr<odRender::Texture> mTexture;
    };

}

#endif
