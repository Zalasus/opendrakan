/*
 * ObjectHandle.cpp
 *
 *  Created on: 12 Nov 2018
 *      Author: zal
 */

#include <odOsg/ObjectHandle.h>

#include <odCore/Exception.h>
#include <odCore/render/Renderer.h>

#include <odOsg/GlmAdapter.h>
#include <odOsg/Renderer.h>
#include <odOsg/Geometry.h>

namespace odOsg
{

    ObjectHandle::ObjectHandle(Renderer *renderer, osg::Group *objectGroup)
    : mObjectGroup(objectGroup)
    , mTransform(new osg::PositionAttitudeTransform)
    , mGeometry(nullptr)
    {
        if(objectGroup == nullptr)
        {
            throw od::Exception("Passed nullptr as object group argument");
        }

        objectGroup->addChild(mTransform);
    }

    ObjectHandle::~ObjectHandle()
    {
        mObjectGroup->removeChild(mTransform);
    }

    void ObjectHandle::setPosition(const glm::vec3 &pos)
    {
        mTransform->setPosition(GlmAdapter::toOsg(pos));
    }

    void ObjectHandle::setOrientation(const glm::quat &orientation)
    {
        mTransform->setAttitude(GlmAdapter::toOsg(orientation));
    }

    void ObjectHandle::setScale(const glm::vec3 &scale)
    {
        mTransform->setScale(GlmAdapter::toOsg(scale));
    }

    odRender::Geometry *ObjectHandle::getGeometry()
    {
        return mGeometry;
    }

    void ObjectHandle::setGeometry(odRender::Geometry *g)
    {
        if(g == nullptr && mGeometry != nullptr)
        {
            mTransform->removeChild(mGeometry->getNode());
            mGeometry = nullptr;
            return;
        }

        mGeometry = dynamic_cast<Geometry*>(g);
        if(mGeometry == nullptr)
        {
            return;
        }

        mTransform->addChild(mGeometry->getNode());
    }

}

