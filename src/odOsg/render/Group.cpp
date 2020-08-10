
#include <odOsg/render/Group.h>

#include <algorithm>

#include <odCore/Exception.h>
#include <odCore/Downcast.h>

#include <odOsg/render/Handle.h>
#include <odOsg/GlmAdapter.h>

namespace odOsg
{

    Group::Group()
    : mParentGroup(nullptr)
    , mTransform(new osg::MatrixTransform())
    {
    }

    Group::~Group()
    {
        if(mParentGroup != nullptr)
        {
            mParentGroup->removeChild(mTransform);
        }
    }

    void Group::addHandle(std::shared_ptr<odRender::Handle> handle)
    {
        OD_CHECK_ARG_NONNULL(handle);

        auto myHandle = od::confident_downcast<Handle>(handle);
        mHandles.push_back(myHandle);

        mTransform->addChild(myHandle->getOsgNode());
    }

    void Group::removeHandle(std::shared_ptr<odRender::Handle> handle)
    {
        OD_CHECK_ARG_NONNULL(handle);

        auto myHandle = od::confident_downcast<Handle>(handle);

        auto it = std::find(mHandles.begin(), mHandles.end(), myHandle);
        if(it != mHandles.end())
        {
            mTransform->removeChild(myHandle->getOsgNode());
            mHandles.erase(it);
        }
    }

    size_t Group::getHandleCount() const
    {
        return mHandles.size();
    }

    std::shared_ptr<odRender::Handle> Group::getHandle(int index)
    {
        return mHandles.at(index);
    }

    void Group::setMatrix(const glm::mat4 &m)
    {
        osg::Matrix osgMatrix = GlmAdapter::toOsg(glm::transpose(m)); // FIXME: this transpose is suspicios...
        mTransform->setMatrix(osgMatrix);
    }

    void Group::setVisible(bool visible)
    {
        int mask = visible ? -1 : 0;
        mTransform->setNodeMask(mask);
    }

}
