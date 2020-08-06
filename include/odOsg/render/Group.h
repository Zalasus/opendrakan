
#ifndef INCLUDE_ODOSG_GROUP_H_
#define INCLUDE_ODOSG_GROUP_H_

#include <osg/MatrixTransform>

#include <odCore/render/Group.h>

namespace odOsg
{
    class Handle;

    class Group : public odRender::Group
    {
    public:

        Group(osg::Group *parent);
        virtual ~Group() override;

        inline osg::Group *getOsgNode() { return mTransform; }

        virtual void addHandle(std::shared_ptr<odRender::Handle> handle) override;
        virtual void removeHandle(std::shared_ptr<odRender::Handle> handle) override;

        virtual void setMatrix(const glm::mat4 &m) override;

        virtual void setVisible(bool visible) override;


    private:

        osg::ref_ptr<osg::Group> mParentGroup;
        std::vector<std::shared_ptr<Handle>> mHandles;

        osg::ref_ptr<osg::MatrixTransform> mTransform;

    };

}

#endif
