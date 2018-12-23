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

#include <odCore/WeakRefPtr.h>

#include <odCore/render/GuiNode.h>

namespace odGui
{
    class Widget;
}

namespace odOsg
{

    class GuiQuad;

    class GuiNode : public odRender::GuiNode
    {
    public:

        GuiNode();
        GuiNode(odGui::Widget *widget);
        virtual ~GuiNode();

        inline osg::Node *getOsgNode() { return mTransform; }

        virtual void addChild(odRender::GuiNode *node) override;
        virtual void removeChild(odRender::GuiNode *node) override;

        virtual void setMatrix(const glm::mat4 &matrix) override;

        virtual void setVisible(bool visible) override;

        virtual odRender::GuiQuad *createGuiQuad() override;
        virtual void removeGuiQuad(odRender::GuiQuad *quad) override;

        void update(float relTime);


    private:

        od::WeakRefPtr<odGui::Widget> mWidget;

        osg::ref_ptr<osg::MatrixTransform> mTransform;
        osg::ref_ptr<osg::Geode> mGeode;
        osg::ref_ptr<osg::Callback> mUpdateCallback;

        std::vector<od::RefPtr<GuiNode>> mChildNodes;
        std::vector<od::RefPtr<GuiQuad>> mGuiQuads;
    };

}

#endif /* INCLUDE_ODOSG_GUINODE_H_ */
