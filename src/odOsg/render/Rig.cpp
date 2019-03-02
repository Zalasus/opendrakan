/*
 * Rig.cpp
 *
 *  Created on: 5 Dec 2018
 *      Author: zal
 */

#include <odOsg/render/Rig.h>

#include <odCore/Exception.h>

#include <odOsg/GlmAdapter.h>
#include <odOsg/Constants.h>

namespace odOsg
{

    Rig::Rig(osg::Node *riggedModelRoot)
    : mRiggedModelRoot(riggedModelRoot)
    , mBoneMatrixUniform(new osg::Uniform(osg::Uniform::FLOAT_MAT4, "bones", Constants::MAX_BONES))
    {
        osg::StateSet *ss = mRiggedModelRoot->getOrCreateStateSet();

        ss->addUniform(mBoneMatrixUniform);
        ss->setDefine("RIGGING");

        for(size_t i = 0; i < mBoneMatrixUniform->getNumElements(); ++i)
        {
            mBoneMatrixUniform->setElement(i, osg::Matrix::identity());
        }
    }

    Rig::~Rig()
    {
        mRiggedModelRoot->getOrCreateStateSet()->removeUniform(mBoneMatrixUniform);
    }

    void Rig::setBoneTransform(size_t boneIndex, glm::mat4 &transform)
    {
        if(boneIndex >= mBoneMatrixUniform->getNumElements())
        {
            throw od::Exception("Bone index passed to renderer exceeds supported number of bones");
        }

        mBoneMatrixUniform->setElement(boneIndex, GlmAdapter::toOsg(transform));
    }

}
