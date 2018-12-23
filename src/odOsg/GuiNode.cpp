/*
 * GuiNode.cpp
 *
 *  Created on: 21 Dec 2018
 *      Author: zal
 */

#include <odOsg/GuiNode.h>

#include <odOsg/GlmAdapter.h>
#include <odOsg/GuiQuad.h>
#include <odOsg/Utils.h>

#include <odCore/gui/Widget.h>

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


    GuiNode::GuiNode()
    : mWidget(nullptr)
    , mTransform(new osg::MatrixTransform)
    , mUpdateCallback(nullptr)
    {
    }

    GuiNode::GuiNode(odGui::Widget *w)
    : mWidget(w)
    , mTransform(new osg::MatrixTransform)
    , mUpdateCallback(new UpdateCallback(this))
    {
        mTransform->addUpdateCallback(mUpdateCallback);
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
        if(node == nullptr || node == this)
        {
            return;
        }

        od::RefPtr<GuiNode> osgNode = upcast<GuiNode>(node);
        mChildNodes.push_back(osgNode);

        mTransform->addChild(osgNode->getOsgNode());
    }

    void GuiNode::removeChild(odRender::GuiNode *node)
    {
        if(node == nullptr || node == this)
        {
            return;
        }

        for(auto it = mChildNodes.begin(); it != mChildNodes.end(); ++it)
        {
            if((*it) == node)
            {
                mTransform->removeChild((*it)->getOsgNode());
                mChildNodes.erase(it);
                break;
            }
        }
    }

    void GuiNode::setMatrix(const glm::mat4 &matrix)
    {
        mTransform->setMatrix(GlmAdapter::toOsg(matrix));
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

        mGuiQuads.emplace_back(new GuiQuad);

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

    void GuiNode::update(float relTime)
    {
        if(mWidget != nullptr)
        {
            mWidget->onUpdate(relTime);
        }
    }

}

