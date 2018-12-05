/*
 * ObjectNode.cpp
 *
 *  Created on: Nov 22, 2018
 *      Author: zal
 */

#include <odOsg/ObjectNode.h>

#include <osg/Depth>

#include <odCore/Exception.h>
#include <odCore/Layer.h>

#include <odOsg/GlmAdapter.h>
#include <odOsg/ModelNode.h>
#include <odOsg/Rig.h>

namespace odOsg
{


    class FrameListenerCallback : public osg::Callback
    {
    public:

        FrameListenerCallback(odRender::FrameListener *fl)
        : mFrameListener(fl)
        , mLastSimTime(0.0)
        , mFirstUpdate(true)
        {
        }

        virtual bool run(osg::Object* object, osg::Object* data) override
        {
            osg::NodeVisitor *nv = data->asNodeVisitor();
            if(nv == nullptr)
            {
                return traverse(object, data);
            }

            const osg::FrameStamp *fs = nv->getFrameStamp();
            if(fs == nullptr)
            {
                return traverse(object, data);
            }

            double simTime = fs->getSimulationTime();

            if(mFirstUpdate)
            {
                mLastSimTime = simTime;
                mFirstUpdate = false;
            }

            if(mFrameListener != nullptr)
            {
                mFrameListener->onFrameUpdate(simTime, simTime-mLastSimTime, fs->getFrameNumber());
            }

            mLastSimTime = simTime;

            return traverse(object, data);
        }


    private:

        odRender::FrameListener *mFrameListener;
        double mLastSimTime;
        bool mFirstUpdate;
    };


    ObjectNode::ObjectNode(Renderer *renderer, osg::Group *objectGroup)
    : mRenderer(renderer)
    , mObjectGroup(objectGroup)
    , mModelNode(nullptr)
    , mFrameListener(nullptr)
    {
        mTransform = new osg::PositionAttitudeTransform;

        mObjectGroup->addChild(mTransform);

        mLightStateCallback = new LightStateCallback(renderer, mTransform, false);
        mTransform->addCullCallback(mLightStateCallback);
    }

    ObjectNode::~ObjectNode()
    {
        mTransform->removeCullCallback(mLightStateCallback);

        if(mUpdateCallback != nullptr)
        {
            mTransform->removeUpdateCallback(mUpdateCallback);
        }

        mObjectGroup->removeChild(mTransform);
    }

    void ObjectNode::setPosition(const glm::vec3 &pos)
    {
        mTransform->setPosition(GlmAdapter::toOsg(pos));
        mLightStateCallback->lightingDirty();
    }

    void ObjectNode::setOrientation(const glm::quat &orientation)
    {
        mTransform->setAttitude(GlmAdapter::toOsg(orientation));
        mLightStateCallback->lightingDirty();
    }

    void ObjectNode::setScale(const glm::vec3 &scale)
    {
        mTransform->setScale(GlmAdapter::toOsg(scale));
        mLightStateCallback->lightingDirty();
    }

    void ObjectNode::setLightingLayer(od::Layer *layer)
    {
        if(layer != nullptr)
        {
            osg::Vec3 diffuse = GlmAdapter::toOsg(layer->getLightColor());
            osg::Vec3 ambient = GlmAdapter::toOsg(layer->getAmbientColor());
            osg::Vec3 direction = GlmAdapter::toOsg(layer->getLightDirection());
            mLightStateCallback->setLayerLight(diffuse, ambient, direction);

        }else
        {
            osg::Vec3 zero(0, 0, 0);
            mLightStateCallback->setLayerLight(zero, zero, zero);
        }
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

    void ObjectNode::setVisible(bool visible)
    {
        mTransform->setNodeMask(visible ? -1 : 0);
    }

    void ObjectNode::setModelPartVisible(size_t partIndex, bool visible)
    {
    }

    void ObjectNode::setRenderMode(RenderMode rm)
    {
        osg::StateSet *ss = mTransform->getOrCreateStateSet();

        switch(rm)
        {
        case RenderMode::Normal:
            if(mDepth != nullptr)
            {
                ss->removeAttribute(mDepth);
                mDepth = nullptr;
            }
            ss->setRenderBinDetails(0, "RenderBin");
            break;

        case RenderMode::Sky:
            {
                if(mDepth == nullptr)
                {
                    mDepth = new osg::Depth;
                    mDepth->setWriteMask(false);
                }
                ss->setAttribute(mDepth, osg::StateAttribute::ON);
                ss->setRenderBinDetails(-1, "RenderBin");
            }

        }
    }

    void ObjectNode::addFrameListener(odRender::FrameListener *listener)
    {
        if(mFrameListener != nullptr)
        {
            throw od::UnsupportedException("Multiple frame listeners unsupported as of now");
        }

        mFrameListener = listener;

        if(mFrameListener != nullptr && mUpdateCallback == nullptr)
        {
            mUpdateCallback = new FrameListenerCallback(mFrameListener);
            mTransform->addUpdateCallback(mUpdateCallback);

        }else if(mFrameListener == nullptr && mUpdateCallback != nullptr)
        {
            mTransform->removeUpdateCallback(mUpdateCallback);
            mUpdateCallback = nullptr;
        }
    }

    void ObjectNode::removeFrameListener(odRender::FrameListener *listener)
    {
        if(mFrameListener != listener)
        {
            return;
        }

        mFrameListener = nullptr;;
    }

    odRender::Rig *ObjectNode::getRig()
    {
        if(mRig == nullptr)
        {
            mRig.reset(new Rig(mTransform));
        }

        return mRig.get();
    }
}


