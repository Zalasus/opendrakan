/*
 * GuINode.h
 *
 *  Created on: 21 Dec 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_GUINODE_H_
#define INCLUDE_ODOSG_GUINODE_H_

#include <osg/MatrixTransform>
#include <osg/Geode>

#include <odCore/render/GuiNode.h>

namespace odOsg
{

    class GuiQuad;

    class GuiNode : public odRender::GuiNode
    {
    public:

        GuiNode();
        virtual ~GuiNode();

        inline osg::Node *getOsgNode() { return mTransform; }

        virtual void addChild(odRender::GuiNode *node) override;
        virtual void removeChild(odRender::GuiNode *node) override;

        virtual void setMatrix(const glm::mat4 &matrix) override;

        virtual void setVisible(bool visible) override;

        virtual void setEnableFrameCallback(bool b) override;

        virtual odRender::GuiQuad *createGuiQuad() override;
        virtual void removeGuiQuad(odRender::GuiQuad *quad) override;


    private:

        osg::ref_ptr<osg::MatrixTransform> mTransform;
        osg::ref_ptr<osg::Geode> mGeode;

        std::vector<od::RefPtr<GuiNode>> mChildNodes;
        std::vector<od::RefPtr<GuiQuad>> mGuiQuads;
    };

}

#endif /* INCLUDE_ODOSG_GUINODE_H_ */
