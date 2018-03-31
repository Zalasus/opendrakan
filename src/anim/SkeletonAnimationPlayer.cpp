/*
 * SkeletonAnimation.cpp
 *
 *  Created on: 25 Mar 2018
 *      Author: zal
 */

#include <anim/SkeletonAnimationPlayer.h>
#include "Logger.h"
#include "ShaderManager.h"
#include "Engine.h"
#include "OdDefines.h"
#include "db/Skeleton.h"

#include <iomanip>

#define OD_MAX_BONE_COUNT 64

namespace od
{

	class CreateAnimatorsVisitor : public osg::NodeVisitor
	{
	public:

		CreateAnimatorsVisitor(osg::Uniform *boneMatrixArray, std::vector<osg::ref_ptr<BoneAnimator>> &animatorList)
        : osg::NodeVisitor(TRAVERSE_ALL_CHILDREN)
		, mAnimatorList(animatorList)
		, mBoneMatrixArray(boneMatrixArray)
        {
        }

        virtual void apply(osg::MatrixTransform &node)
        {
        	BoneNode *bn = dynamic_cast<BoneNode*>(&node);
        	if(bn != nullptr)
        	{
				osg::ref_ptr<BoneAnimator> animator(new BoneAnimator(bn, mBoneMatrixArray));
				mAnimatorList.push_back(animator);
        	}

        	traverse(node);
        }


	private:

        std::vector<osg::ref_ptr<BoneAnimator>> &mAnimatorList;
        osg::ref_ptr<osg::Uniform> mBoneMatrixArray;
	};

	SkeletonAnimationPlayer::SkeletonAnimationPlayer(Engine &engine, osg::Node *modelNode, osg::Group *skeletonRoot)
	: mEngine(engine)
	, mModelNode(modelNode)
	, mSkeletonRoot(skeletonRoot)
	, mBoneMatrixArray(new osg::Uniform(osg::Uniform::FLOAT_MAT4, "bones", OD_MAX_BONE_COUNT))
	, mPlayState(AnimationPlayState::STOPPED)
	{
		// create one animator for each MatrixTransform child of group
		CreateAnimatorsVisitor cav(mBoneMatrixArray, mAnimators);
		mSkeletonRoot->accept(cav);

		Logger::debug() << "Created SkeletonAnimation with " << mAnimators.size() << " animators";

		// attach rigging shader to model node TODO: is this the right place to attach this? how to we ensure it is not overwritten?
		osg::ref_ptr<osg::Shader> riggingShader = mEngine.getShaderManager().loadShader(OD_SHADER_RIGGED_VERTEX, osg::Shader::VERTEX);
		mRiggingProgram = mEngine.getShaderManager().makeProgram(riggingShader, nullptr);
		mRiggingProgram->addBindAttribLocation("influencingBones", 13);
		mRiggingProgram->addBindAttribLocation("vertexWeights", 14);
		mModelNode->getOrCreateStateSet()->setAttribute(mRiggingProgram, osg::StateAttribute::ON);
		mModelNode->getOrCreateStateSet()->addUniform(mBoneMatrixArray, osg::StateAttribute::ON);

		for(size_t i = 0; i < OD_MAX_BONE_COUNT; ++i)
		{
			mBoneMatrixArray->setElement(i, osg::Matrixf::identity());
		}
	}

	SkeletonAnimationPlayer::~SkeletonAnimationPlayer()
	{
		mModelNode->getOrCreateStateSet()->removeAttribute(mRiggingProgram);
	}

	void SkeletonAnimationPlayer::setAnimation(osg::ref_ptr<Animation> anim)
	{
		if(anim->getModelNodeCount() != mAnimators.size())
		{
			Logger::warn() << "Animation '" << anim->getName() << "' requires different number of nodes than controlled by animator";
		}

		mCurrentAnimation = anim;

		for(auto it = mAnimators.begin(); it != mAnimators.end(); ++it)
		{
			int32_t jointIndex = (*it)->getBoneNode()->getJointInfoIndex();

			Animation::AnimStartEndPair startEnd = mCurrentAnimation->getKeyframesForNode(jointIndex);

			(*it)->setKeyframes(startEnd.first, startEnd.second);
		}
	}

	void SkeletonAnimationPlayer::setPlayState(AnimationPlayState state)
	{
		mPlayState = state;

		for(auto it = mAnimators.begin(); it != mAnimators.end(); ++it)
		{
			(*it)->setPlayState(state);
		}
	}

}
