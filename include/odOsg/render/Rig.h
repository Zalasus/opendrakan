/*
 * Rig.h
 *
 *  Created on: 5 Dec 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_RIG_H_
#define INCLUDE_ODOSG_RIG_H_

#include <osg/Node>
#include <osg/Uniform>

#include <odCore/render/Rig.h>

namespace odOsg
{

    class Rig : public odRender::Rig
    {
    public:

        Rig(osg::Node *riggedModelRoot);
        virtual ~Rig();

        virtual void setBoneTransform(size_t boneIndex, glm::mat4 &transform) override;


    private:

        osg::ref_ptr<osg::Node> mRiggedModelRoot;
        osg::ref_ptr<osg::Uniform> mBoneMatrixUniform;
    };

}

#endif /* INCLUDE_ODOSG_RIG_H_ */
