/*
 * RenderNode.h
 *
 *  Created on: May 10, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_RENDERNODE_H_
#define INCLUDE_ODCORE_RENDER_RENDERNODE_H_

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <mutex>

#include <odCore/RefCounted.h>

namespace odRender
{

    class ObjectNode;

    /**
     * A common interface for render nodes. This provides methods all render nodes should have
     * and is implemented by all subinterfaces. There is no need for a renderer to provide an
     * implementation for this directly.
     */
    class RenderNode : public od::RefCounted
    {
    public:

        /**
         * Provides a mutex that can be used to synchronize access to this node with the
         * render thread. Note that this will already be locked during any renderer-provided
         * callbacks. Trying trying to lock it again inside a callback will cause a deadlock.
         */
        virtual std::mutex &getMutex() = 0;

        virtual glm::vec3 getPosition() = 0;
        virtual glm::quat getOrientation() = 0;
        virtual glm::vec3 getScale() = 0;

        virtual void setPosition(const glm::vec3 &pos) = 0;
        virtual void setOrientation(const glm::quat &orientation) = 0;
        virtual void setScale(const glm::vec3 &scale) = 0;

        // fast downcast methods.
        virtual ObjectNode *asObjectNode();

    };

}


#endif /* INCLUDE_ODCORE_RENDER_RENDERNODE_H_ */
