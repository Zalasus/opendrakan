/*
 * ObjectNode.cpp
 *
 *  Created on: Nov 22, 2018
 *      Author: zal
 */

#include <odOsg/ObjectNode.h>

#include <odCore/Exception.h>

#include <odOsg/GlmAdapter.h>
#include <odOsg/ModelNode.h>

namespace odOsg
{

    ObjectNode::ObjectNode(Renderer *renderer, osg::Group *objectGroup)
    : mRenderer(renderer)
    , mObjectGroup(objectGroup)
    , mModelNode(nullptr)
    {
        mTransform = new osg::PositionAttitudeTransform;

        mObjectGroup->addChild(mTransform);

        mLightStateCallback = new LightStateCallback(renderer, mTransform, false);
        mTransform->addCullCallback(mLightStateCallback);
    }

    ObjectNode::~ObjectNode()
    {
        mTransform->removeCullCallback(mLightStateCallback);

        mObjectGroup->removeChild(mTransform);
    }

    void ObjectNode::setPosition(const glm::vec3 &pos)
    {
        mTransform->setPosition(GlmAdapter::toOsg(pos));
    }

    void ObjectNode::setOrientation(const glm::quat &orientation)
    {
        mTransform->setAttitude(GlmAdapter::toOsg(orientation));
    }

    void ObjectNode::setScale(const glm::vec3 &scale)
    {
        mTransform->setScale(GlmAdapter::toOsg(scale));
    }

    odRender::ModelNode *ObjectNode::getModel()
    {
        return mModelNode;
    }

    void ObjectNode::setModel(odRender::ModelNode *node)
    {
        if(mModelNode != nullptr)
        {
            mTransform->removeChild(mModelNode->getOsgNode());
        }

        if(node == nullptr)
        {
            mModelNode = nullptr;
            return;
        }

        ModelNode *osgModelNode = dynamic_cast<ModelNode*>(node);
        if(osgModelNode == nullptr)
        {
            throw od::Exception("Passed model node was no odOsg::ModelNode");
        }

        mModelNode = osgModelNode;

        mTransform->addChild(mModelNode->getOsgNode());
    }

    void ObjectNode::showModelPart(size_t index)
    {

    }

    void ObjectNode::hideModelPart(size_t index)
    {

    }

}


