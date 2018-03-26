/*
 * SkeletonAnimation.cpp
 *
 *  Created on: 25 Mar 2018
 *      Author: zal
 */

#include "anim/SkeletonAnimation.h"

#include "Logger.h"
#include "ShaderManager.h"
#include "Engine.h"
#include "OdDefines.h"
#include "db/Skeleton.h"

#define OD_MAX_BONE_COUNT 64

namespace od
{

	class UploadBoneCallback : public osg::NodeCallback
	{
	public:

		UploadBoneCallback(BoneNode *bn, osg::Uniform *boneMatrixList)
		: mBoneNode(bn)
		, mBoneMatrixList(boneMatrixList)
		{
		}

		virtual void operator()(osg::Node *node, osg::NodeVisitor *nv)
		{
			if(node != mBoneNode)
			{
				return;
			}

			mBoneMatrixList->setElement(mBoneNode->getJointInfoIndex(), mBoneNode->getMatrix());

			traverse(node, nv);
		}

	private:

		osg::ref_ptr<BoneNode> mBoneNode;
		osg::ref_ptr<osg::Uniform> mBoneMatrixList;
	};

	class CreateAnimatorsVisitor : public osg::NodeVisitor
	{
	public:

		CreateAnimatorsVisitor(osg::Uniform *boneMatrixList, std::vector<osg::ref_ptr<Animator>> &animatorList)
        : osg::NodeVisitor(TRAVERSE_ALL_CHILDREN)
		, mAnimatorList(animatorList)
		, mBoneMatrixList(boneMatrixList)
        {
        }

        virtual void apply(osg::MatrixTransform &node)
        {
        	BoneNode *bn = dynamic_cast<BoneNode*>(&node);
        	if(bn != nullptr)
        	{
				osg::ref_ptr<Animator> animator(new Animator(&node));
				mAnimatorList.push_back(animator);

				bn->addUpdateCallback(new UploadBoneCallback(bn, mBoneMatrixList));
        	}

        	traverse(node);
        }

	private:

        std::vector<osg::ref_ptr<Animator>> &mAnimatorList;
        osg::ref_ptr<osg::Uniform> mBoneMatrixList;
	};

	SkeletonAnimation::SkeletonAnimation(Engine &engine, osg::Node *modelNode, osg::Group *skeletonRoot)
	: mEngine(engine)
	, mModelNode(modelNode)
	, mSkeletonRoot(skeletonRoot)
	, mBoneMatrixList(new osg::Uniform(osg::Uniform::FLOAT_MAT4, "bones", OD_MAX_BONE_COUNT))
	{
		// create one animator for each MatrixTransform child of group
		CreateAnimatorsVisitor cav(mBoneMatrixList, mAnimators);
		mSkeletonRoot->accept(cav);

		// attach rigging shader to model node TODO: is this the right place to attach this? how to we ensure it is not overwritten?
		osg::ref_ptr<osg::Shader> riggingShader = mEngine.getShaderManager().loadShader(OD_SHADER_RIGGED_VERTEX, osg::Shader::VERTEX);
		mRiggingProgram = mEngine.getShaderManager().makeProgram(riggingShader, nullptr);
		mModelNode->getOrCreateStateSet()->setAttribute(mRiggingProgram, osg::StateAttribute::ON);
		mModelNode->getOrCreateStateSet()->addUniform(mBoneMatrixList, osg::StateAttribute::ON);

		Logger::debug() << "Created SkeletonAnimation with " << mAnimators.size() << " animators";
	}

	SkeletonAnimation::~SkeletonAnimation()
	{
		mModelNode->getOrCreateStateSet()->removeAttribute(mRiggingProgram);
	}

}
