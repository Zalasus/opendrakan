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

namespace odOsg
{

    GuiNode::GuiNode()
    : mTransform(new osg::MatrixTransform)
    {
    }

    GuiNode::~GuiNode()
    {
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

    void GuiNode::setEnableFrameCallback(bool b)
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

}

