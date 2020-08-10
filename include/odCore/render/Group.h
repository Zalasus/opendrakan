
#ifndef INCLUDE_ODCORE_RENDER_GROUP_H_
#define INCLUDE_ODCORE_RENDER_GROUP_H_

#include <memory>
#include <glm/mat4x4.hpp>

namespace odRender
{
    class Handle;

    class Group
    {
    public:

        virtual ~Group() = default;

        virtual void addHandle(std::shared_ptr<Handle> handle) = 0;
        virtual void removeHandle(std::shared_ptr<Handle> handle) = 0;
        virtual size_t getHandleCount() = 0;
        virtual std::shared_ptr<Handle> getHandle(int index) = 0;

        virtual void setMatrix(const glm::mat4 &m) = 0;
        virtual void setRenderOrderHint(size_t i) = 0;
        virtual void setVisible(bool visible) = 0;

    };

}

#endif
