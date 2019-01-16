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

        GuiNode(osg::Group *guiRoot, odGui::Widget *widget);
        virtual ~GuiNode();

        virtual void setMatrix(const glm::mat4 &m) override;
        virtual void setViewport(const glm::vec2 &offset, const glm::vec2 &size) override;

        virtual void setOrthogonalMode() override;
        virtual void setPerspectiveMode(float fov, float aspect) override;

        virtual void setVisible(bool visible) override;

        virtual void setZIndex(int32_t zIndex) override;
        virtual void reorderChildren() override;

        virtual odRender::GuiQuad *createGuiQuad() override;
        virtual void removeGuiQuad(odRender::GuiQuad *quad) override;

        virtual odRender::ObjectNode *createObjectNode() override;
        virtual void removeObjectNode(odRender::ObjectNode *node) override;

        void update(float relTime);


    private:

        osg::ref_ptr<osg::Group> mGuiRoot;
        od::WeakRefPtr<odGui::Widget> mWidget;

        osg::ref_ptr<osg::MatrixTransform> mTransform;
        osg::ref_ptr<osg::Geode> mGeode;
        osg::ref_ptr<osg::Callback> mUpdateCallback;

        std::vector<od::RefPtr<GuiQuad>> mGuiQuads;
    };

}

#endif /* INCLUDE_ODOSG_GUINODE_H_ */
