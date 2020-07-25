/*
 * GuiNode.cpp
 *
 *  Created on: 21 Dec 2018
 *      Author: zal
 */

#include <odOsg/render/GuiNode.h>

#include <algorithm>

#include <odCore/Downcast.h>

#include <odCore/gui/Widget.h>

#include <odOsg/GlmAdapter.h>
#include <odOsg/render/GuiQuad.h>
#include <odOsg/render/Handle.h>

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


    GuiNode::GuiNode(Renderer &renderer, std::shared_ptr<odGui::Widget> w)
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

        for(auto &c : mChildHandles)
        {
            mObjectGroup->removeChild(c->getOsgNode());
        }
    }

    void GuiNode::addChild(std::shared_ptr<odRender::GuiNode> node)
    {
        if(node == nullptr)
        {
            return;
        }

        std::shared_ptr<GuiNode> guiNode = od::confident_downcast<GuiNode>(node);
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

        mGuiQuads.push_back(std::make_shared<GuiQuad>());

        mGeode->addDrawable(mGuiQuads.back()->getOsgGeometry());

        return mGuiQuads.back().get();
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

    void GuiNode::addChildHandle(std::shared_ptr<odRender::Handle> handle)
    {
        if(handle == nullptr)
        {
            return;
        }

        if(mObjectGroup == nullptr)
        {
            mObjectGroup = new osg::Group;
            osg::StateSet *ss = mObjectGroup->getOrCreateStateSet();
            ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
            ss->setMode(GL_CULL_FACE, osg::StateAttribute::ON);
            mTransform->addChild(mObjectGroup);
        }

        auto osgHandle = od::confident_downcast<Handle>(handle);

        mObjectGroup->addChild(osgHandle->getOsgNode());

        mChildHandles.push_back(osgHandle);
    }

    void GuiNode::removeChildHandle(odRender::Handle *handle)
    {
        if(handle == nullptr)
        {
            return;
        }

        auto osgHandle = od::confident_downcast<Handle>(handle);

        auto it = std::find(mChildHandles.begin(), mChildHandles.end(), osgHandle);
        if(it != mChildHandles.end())
        {
            mObjectGroup->removeChild(osgHandle->getOsgNode());

            mChildHandles.erase(it);
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
