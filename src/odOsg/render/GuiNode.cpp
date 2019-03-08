/*
 * GuiNode.cpp
 *
 *  Created on: 21 Dec 2018
 *      Author: zal
 */

#include <odOsg/render/GuiNode.h>

#include <algorithm>

#include <odCore/Upcast.h>

#include <odCore/gui/Widget.h>

#include <odOsg/GlmAdapter.h>
#include <odOsg/render/GuiQuad.h>
#include <odOsg/render/ObjectNode.h>

namespace odOsg
{

    class UpdateCallback : public osg::Callback
    {
    public:

        UpdateCallback(GuiNode *n)
        : mNode(n)
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

            if(mNode != nullptr)
            {
                mNode->update(simTime - mLastSimTime);
            }

            mLastSimTime = simTime;

            return traverse(object, data);
        }


    private:

        GuiNode *mNode;
        double mLastSimTime;
        bool mFirstUpdate;
    };


    GuiNode::GuiNode(Renderer *renderer, odGui::Widget *w)
    : mRenderer(renderer)
    , mWidget(w)
    , mTransform(new osg::MatrixTransform)
    {
        if(w != nullptr)
        {
            mUpdateCallback = new UpdateCallback(this);
            mTransform->addUpdateCallback(mUpdateCallback);
        }

        mTransform->getOrCreateStateSet()->setNestRenderBins(true);
    }

    GuiNode::~GuiNode()
    {
        if(mUpdateCallback != nullptr)
        {
            mTransform->removeUpdateCallback(mUpdateCallback);
        }
    }

    void GuiNode::addChild(odRender::GuiNode *node)
    {
        if(node == nullptr)
        {
            return;
        }

        od::RefPtr<GuiNode> guiNode = od::confident_upcast<GuiNode>(node);
        mChildren.push_back(guiNode);

        mTransform->addChild(guiNode->getOsgNode());
    }

    void GuiNode::removeChild(odRender::GuiNode *node)
    {
        GuiNode *gn = static_cast<GuiNode*>(node); // static cast should suffice. we'll just use it to compare adresses anyway
        auto it = std::find(mChildren.begin(), mChildren.end(), gn);
        if(it != mChildren.end())
        {
            mTransform->removeChild((*it)->getOsgNode());
            mChildren.erase(it);
        }
    }

    void GuiNode::setMatrix(const glm::mat4 &m)
    {
        mTransform->setMatrix(GlmAdapter::toOsg(m));
    }

    void GuiNode::setViewport(const glm::vec2 &offset, const glm::vec2 &size)
    {

    }

    void GuiNode::setOrthogonalMode()
    {

    }

    void GuiNode::setPerspectiveMode(float fov, float aspect)
    {

    }

    void GuiNode::setVisible(bool visible)
    {
        mTransform->setNodeMask(visible ? 1 : 0);
    }

    void GuiNode::setZIndex(int32_t zIndex)
    {
        mTransform->getOrCreateStateSet()->setRenderBinDetails(-zIndex, "RenderBin");
    }

    void GuiNode::reorderChildren()
    {
    }

    odRender::GuiQuad *GuiNode::createGuiQuad()
    {
        if(mGeode == nullptr)
        {
            mGeode = new osg::Geode;
            mTransform->addChild(mGeode);
        }

        mGuiQuads.push_back(od::make_refd<GuiQuad>());

        mGeode->addDrawable(mGuiQuads.back()->getOsgGeometry());

        return mGuiQuads.back();
    }

    void GuiNode::removeGuiQuad(odRender::GuiQuad *quad)
    {
        if(mGeode == nullptr)
        {
            return;
        }

        for(auto it = mGuiQuads.begin(); it != mGuiQuads.end(); ++it)
        {
            if((*it) == quad)
            {
                mGeode->removeDrawable((*it)->getOsgGeometry());
                mGuiQuads.erase(it);
                break;
            }
        }
    }

    odRender::ObjectNode *GuiNode::createObjectNode()
    {
        if(mObjectGroup == nullptr)
        {
            mObjectGroup = new osg::Group;
            osg::StateSet *ss = mObjectGroup->getOrCreateStateSet();
            ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
            ss->setMode(GL_CULL_FACE, osg::StateAttribute::ON);
            mTransform->addChild(mObjectGroup);
        }

        auto node = od::make_refd<ObjectNode>(mRenderer, mObjectGroup);

        node->setLocalLightMask(0); // usually, we don't want local lights to illuminate the GUI

        mObjectNodes.push_back(node);

        return node.get();
    }

    void GuiNode::removeObjectNode(odRender::ObjectNode *node)
    {
        ObjectNode *on = static_cast<ObjectNode*>(node);
        auto it = std::find(mObjectNodes.begin(), mObjectNodes.end(), on);
        if(it != mObjectNodes.end())
        {
            mObjectNodes.erase(it);
        }
    }

    void GuiNode::update(float relTime)
    {
        if(mWidget.isNonNull())
        {
            mWidget.aquire()->update(relTime);
        }
    }

}

