/*
 * PhysicsDebugDrawer.h
 *
 *  Created on: 23 Feb 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_PHYSICSDEBUGDRAWER_H_
#define INCLUDE_ODCORE_RENDER_PHYSICSDEBUGDRAWER_H_

#include <glm/vec3.hpp>

namespace odRender
{

    /**
     * A simple inteface to provide the abstraction necessary for renderer and physics system independent
     * debug drawing. While this uses an immediate-mode style way of drawing lines, the implementation should
     * not actually use immediate mode. The start and end calls are there to help synchronization of rendering,
     * which might use a slower framerate or be initiated from a different thread.
     */
    class PhysicsDebugDrawer
    {
    public:

        virtual ~PhysicsDebugDrawer() = default;

        virtual void startDrawing() = 0;
        virtual void drawLine(const glm::vec3 &start, const glm::vec3 &end, const glm::vec3 &color) = 0;
        virtual void endDrawing() = 0;
    };

}

#endif /* INCLUDE_ODCORE_RENDER_PHYSICSDEBUGDRAWER_H_ */
