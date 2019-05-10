/*
 * ObjectNode.h
 *
 *  Created on: Nov 19, 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_OBJECTNODE_H_
#define INCLUDE_ODCORE_RENDER_OBJECTNODE_H_

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <odCore/render/RenderNode.h>

namespace od
{
    class Light;
}

namespace odRender
{
    class ModelNode;
    class Rig;

    class FrameListener
    {
    public:

        virtual ~FrameListener() = default;

        virtual void onFrameUpdate(double simTime, double relTime, uint32_t frameNumber) = 0;

    };


    /**
     * A render node for rendering a ModelNode.
     */
    class ObjectNode : public odRender::RenderNode
    {
    public:

        enum class RenderMode
        {
            Normal,
            Sky
        };

        virtual void setGlobalLight(const glm::vec3 &direction, const glm::vec3 &diffuse, const glm::vec3 &ambient) = 0;
        virtual void setLocalLightMask(uint32_t localLightMask) = 0;

        virtual ModelNode *getModel() = 0;
        virtual void setModel(ModelNode *node) = 0;

        virtual void setVisible(bool visible) = 0;
        virtual void setModelPartVisible(size_t partIndex, bool visible) = 0;

        virtual void setRenderMode(RenderMode rm) = 0;

        virtual void addFrameListener(FrameListener *listener) = 0;
        virtual void removeFrameListener(FrameListener *listener) = 0;

        virtual void setEnableColorModifier(bool b) = 0;
        virtual void setColorModifier(const glm::vec4 &cm) = 0;

        virtual Rig *getRig() = 0;

        virtual void addLight(od::Light *light) = 0;
        virtual void removeLight(od::Light *light) = 0;
        virtual void clearLightList() = 0;
    };

}


#endif /* INCLUDE_ODCORE_RENDER_OBJECTNODE_H_ */
