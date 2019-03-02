/*
 * ObjectNode.cpp
 *
 *  Created on: Nov 22, 2018
 *      Author: zal
 */

#include <odOsg/render/ObjectNode.h>

#include <osg/Depth>

#include <odCore/Exception.h>
#include <odCore/Layer.h>

#include <odOsg/Constants.h>
#include <odOsg/GlmAdapter.h>
#include <odOsg/render/ModelNode.h>
#include <odOsg/render/Rig.h>

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

        mLightStateAttribute = new LightStateAttribute(renderer, Constants::MAX_LIGHTS);
        mTransform->getOrCreateStateSet()->setAttribute(mLightStateAttribute, osg::StateAttribute::ON);
    }

    ObjectNode::~ObjectNode()
    {
        if(mUpdateCallback != nullptr)
        {
            mTransform->removeUpdateCallback(mUpdateCallback);
        }

        mObjectGroup->removeChild(mTransform);
    }

    glm::vec3 ObjectNode::getPosition()
    {
        return GlmAdapter::toGlm(mTransform->getPosition());
    }

    glm::quat ObjectNode::getOrientation()
    {
        return GlmAdapter::toGlm(mTransform->getAttitude());
    }

    glm::vec3 ObjectNode::getScale()
    {
        return GlmAdapter::toGlm(mTransform->getScale());
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

    void ObjectNode::setGlobalLight(const glm::vec3 &diffuse, const glm::vec3 &ambient, const glm::vec3 &direction)
    {
        osg::Vec3 diff = GlmAdapter::toOsg(diffuse);
        osg::Vec3 amb = GlmAdapter::toOsg(ambient);
        osg::Vec3 dir = GlmAdapter::toOsg(direction);
        mLightStateAttribute->setLayerLight(diff, amb, dir);
    }

    void ObjectNode::setLocalLightMask(uint32_t localLightMask)
    {
        //mLightStateCallback->setLightMask(localLightMask);
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

    void ObjectNode::setEnableColorModifier(bool enable)
    {
        if(enable && mColorModifierUniform == nullptr)
        {
            mColorModifierUniform = new osg::Uniform("colorModifier", osg::Vec4(1.0, 1.0, 1.0, 1.0));

            osg::StateSet *ss = mTransform->getOrCreateStateSet();
            ss->setDefine("COLOR_MODIFIER");
            ss->addUniform(mColorModifierUniform);

        }else if(!enable && mColorModifierUniform != nullptr)
        {
            osg::StateSet *ss = mTransform->getOrCreateStateSet();
            ss->removeDefine("COLOR_MODIFIER");
            ss->removeUniform(mColorModifierUniform);

            mColorModifierUniform = nullptr;
        }
    }

    void ObjectNode::setColorModifier(const glm::vec4 &cm)
    {
        if(mColorModifierUniform != nullptr)
        {
            mColorModifierUniform->set(GlmAdapter::toOsg(cm));
        }
    }

    odRender::Rig *ObjectNode::getRig()
    {
        if(mRig == nullptr)
        {
            mRig = std::make_unique<Rig>(mTransform);
        }

        return mRig.get();
    }

    void ObjectNode::addLight(od::Light *light)
    {
        mLightStateAttribute->addLight(light);
    }

    void ObjectNode::removeLight(od::Light *light)
    {
        mLightStateAttribute->removeLight(light);
    }

    void ObjectNode::clearLightList()
    {
        mLightStateAttribute->clearLightList();
    }
}


