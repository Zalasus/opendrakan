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

namespace odOsg
{

    ObjectHandle::ObjectHandle(Renderer *renderer, osg::Group *objectGroup)
    : odRender::Handle(renderer)
    , mTransform(new osg::PositionAttitudeTransform)
    {
        if(objectGroup == nullptr)
        {
            throw od::Exception("Passed nullptr as object group argument");
        }

        objectGroup->addChild(mTransform);
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
        return nullptr; // must be aquired via model instead
    }



}

