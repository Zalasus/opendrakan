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

namespace odRender
{

    class ModelNode;

    class ObjectNode : public od::RefCounted
    {
    public:

        virtual ~ObjectNode() = default;

        virtual void setPosition(const glm::vec3 &pos) = 0;
        virtual void setOrientation(const glm::quat &orientation) = 0;
        virtual void setScale(const glm::vec3 &scale) = 0;

        virtual ModelNode *getModel() = 0;
        virtual void setModel(ModelNode *node) = 0;
        virtual void showModelPart(size_t index) = 0;
        virtual void hideModelPart(size_t index) = 0;
    };

}


#endif /* INCLUDE_ODCORE_RENDER_OBJECTNODE_H_ */
