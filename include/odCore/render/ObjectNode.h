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

#include <odCore/RefCounted.h>

namespace od
{
    class Layer;
}

namespace odRender
{

    class ModelNode;


    class FrameListener
    {
    public:

        virtual ~FrameListener() = default;

        virtual void onFrameUpdate(double simTime, double relTime, uint32_t frameNumber) = 0;

    };


    class ObjectNode : public od::RefCounted
    {
    public:

        enum class RenderMode
        {
            Normal,
            Sky
        };

        virtual ~ObjectNode() = default;

        virtual void setPosition(const glm::vec3 &pos) = 0;
        virtual void setOrientation(const glm::quat &orientation) = 0;
        virtual void setScale(const glm::vec3 &scale) = 0;
        virtual void setLightingLayer(od::Layer *layer) = 0;

        virtual ModelNode *getModel() = 0;
        virtual void setModel(ModelNode *node) = 0;

        virtual void setVisible(bool visible) = 0;
        virtual void setModelPartVisible(size_t partIndex, bool visible) = 0;

        virtual void setRenderMode(RenderMode rm) = 0;

        virtual void addFrameListener(FrameListener *listener) = 0;
        virtual void removeFrameListener(FrameListener *listener) = 0;
    };

}


#endif /* INCLUDE_ODCORE_RENDER_OBJECTNODE_H_ */
