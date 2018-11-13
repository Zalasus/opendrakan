/*
 * ObjectHandle.h
 *
 *  Created on: 12 Nov 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_OBJECTHANDLE_H_
#define INCLUDE_ODOSG_OBJECTHANDLE_H_

#include <odCore/render/Handle.h>

#include <osg/PositionAttitudeTransform>

namespace odOsg
{

    class Renderer;

    class ObjectHandle : public odRender::Handle
    {
    public:

        ObjectHandle(Renderer *renderer, osg::Group *objectGroup);
        virtual ~ObjectHandle();

        virtual void setPosition(const glm::vec3 &pos) override;
        virtual void setOrientation(const glm::quat &orientation) override;
        virtual void setScale(const glm::vec3 &scale) override;
        virtual odRender::Geometry *getGeometry() override;


    private:

        osg::ref_ptr<osg::Group> mObjectGroup;
        osg::ref_ptr<osg::PositionAttitudeTransform> mTransform;

    };

}

#endif /* INCLUDE_ODOSG_OBJECTHANDLE_H_ */
