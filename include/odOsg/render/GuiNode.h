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
    class Renderer;
    class Handle;

    class GuiNode : public odRender::GuiNode
    {
    public:

        GuiNode(Renderer &renderer);
        virtual ~GuiNode();

        inline osg::Node *getOsgNode() { return mTransform; }

        virtual void addChild(std::shared_ptr<odRender::GuiNode> node) override;
        virtual void removeChild(std::shared_ptr<odRender::GuiNode> node) override;

        virtual void setMatrix(const glm::mat4 &m) override;
        virtual void setViewport(const glm::vec2 &offset, const glm::vec2 &size) override;

        virtual void setOrthogonalMode() override;
        virtual void setPerspectiveMode(float fov, float aspect) override;

        virtual void setVisible(bool visible) override;

        virtual void setZIndex(int32_t zIndex) override;
        virtual void reorderChildren() override;

        virtual std::shared_ptr<odRender::GuiQuad> createGuiQuad() override;
        virtual void removeGuiQuad(std::shared_ptr<odRender::GuiQuad> quad) override;

        virtual void addChildHandle(std::shared_ptr<odRender::Handle> handle) override;
        virtual void removeChildHandle(std::shared_ptr<odRender::Handle> handle) override;


        void update(float relTime);


    private:

        Renderer &mRenderer;

        osg::ref_ptr<osg::MatrixTransform> mTransform;
        osg::ref_ptr<osg::Geode> mGeode;
        osg::ref_ptr<osg::Callback> mUpdateCallback;
        osg::ref_ptr<osg::Group> mObjectGroup;

        std::vector<std::shared_ptr<GuiNode>> mChildren;
        std::vector<std::shared_ptr<GuiQuad>> mGuiQuads;
        std::vector<std::shared_ptr<Handle>> mChildHandles;
    };

}

#endif /* INCLUDE_ODOSG_GUINODE_H_ */
