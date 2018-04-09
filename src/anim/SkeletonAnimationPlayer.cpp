/*
 * SkeletonAnimation.cpp
 *
 *  Created on: 25 Mar 2018
 *      Author: zal
 */

#include "anim/SkeletonAnimationPlayer.h"

#include <osg/Matrix>

#include "Logger.h"
#include "ShaderManager.h"
#include "Engine.h"
#include "OdDefines.h"
#include "db/Skeleton.h"

namespace od
{
	class CreateAnimatorsVisitor : public osg::NodeVisitor
	{
	public:

		CreateAnimatorsVisitor(std::vector<osg::ref_ptr<Animator>> &animatorList, osg::PositionAttitudeTransform *accumXform)
        : osg::NodeVisitor(TRAVERSE_ALL_CHILDREN)
		, mAnimatorList(animatorList)
		, mAccumulatingXform(accumXform)
        {
        }

        virtual void apply(osg::MatrixTransform &node)
        {
        	BoneNode *bn = dynamic_cast<BoneNode*>(&node);
        	if(bn != nullptr)
        	{
				osg::ref_ptr<Animator> animator(new Animator(bn));
				mAnimatorList.push_back(animator);

				if(mAccumulatingXform != nullptr && bn->isRoot())
				{
					animator->setAccumulatingXform(mAccumulatingXform);
				}
        	}

        	traverse(node);
        }


	private:

        std::vector<osg::ref_ptr<Animator>> &mAnimatorList;
        osg::ref_ptr<osg::PositionAttitudeTransform> mAccumulatingXform;
	};



	class BoneUploadVisitor : public osg::NodeVisitor
	{
	public:

		BoneUploadVisitor(osg::Uniform *boneMatrixArray)
        : osg::NodeVisitor(TRAVERSE_ALL_CHILDREN)
		, mBoneMatrixArray(boneMatrixArray)
		, mCurrentMatrix(osg::Matrixd::identity())
        {
        }

        virtual void apply(osg::MatrixTransform &node)
        {
        	osg::Matrixd prevMatrix = mCurrentMatrix; // since i don't want to create a stack for that yet

        	BoneNode *bn = dynamic_cast<BoneNode*>(&node);
        	if(bn != nullptr)
        	{
        		mCurrentMatrix.preMult(bn->getMatrix() * bn->getInverseBindPoseXform());
        		mBoneMatrixArray->setElement(bn->getJointInfoIndex(), mCurrentMatrix);

        	}else
        	{
        		// handle non-bone transforms in bone tree the normal way, as we would ignore them completely otherwise
        		node.computeLocalToWorldMatrix(mCurrentMatrix, this);
        	}

        	traverse(node);

        	mCurrentMatrix = prevMatrix;
        }


	private:

        osg::ref_ptr<osg::Uniform> mBoneMatrixArray;
        osg::Matrixd mCurrentMatrix;
	};



	class BoneUploadCallback : public osg::NodeCallback
	{
	public:

		BoneUploadCallback(osg::Uniform *boneMatrixArray)
		: mBoneMatrixArray(boneMatrixArray)
		{
		}

		virtual void operator()(osg::Node *node, osg::NodeVisitor *nv)
		{
			// first let update visitor traverse so all attached Animator object get updated
			//  before we start to upload the new bone positions
			traverse(node, nv);

			BoneUploadVisitor buv(mBoneMatrixArray); // TODO: no need to create this everytime. maybe add as member and just call reset here
			node->accept(buv);
		}


	private:

		 osg::ref_ptr<osg::Uniform> mBoneMatrixArray;;
	};



	SkeletonAnimationPlayer::SkeletonAnimationPlayer(Engine &engine, osg::Node *objectRoot, osg::Group *skeletonRoot, osg::PositionAttitudeTransform *accumulatingXform)
	: mEngine(engine)
	, mObjectRoot(objectRoot)
	, mSkeletonRoot(skeletonRoot)
	, mAccumulatingXform(accumulatingXform)
	, mBoneMatrixArray(new osg::Uniform(osg::Uniform::FLOAT_MAT4, "bones", OD_MAX_BONE_COUNT))
	, mUploadCallback(new BoneUploadCallback(mBoneMatrixArray))
	{
		// create one animator for each MatrixTransform child of group
		CreateAnimatorsVisitor cav(mAnimators, mAccumulatingXform);
		mSkeletonRoot->accept(cav);

		Logger::debug() << "Created SkeletonAnimation with " << mAnimators.size() << " animators";

		// attach rigging shader to model node TODO: is this the right place to attach this? how to we ensure it is not overwritten?
		osg::ref_ptr<osg::Shader> riggingShader = mEngine.getShaderManager().loadShader(OD_SHADER_RIGGED_VERTEX, osg::Shader::VERTEX);
		mRiggingProgram = mEngine.getShaderManager().makeProgram(riggingShader, nullptr);
		mRiggingProgram->addBindAttribLocation("influencingBones", 13);
		mRiggingProgram->addBindAttribLocation("vertexWeights", 14);
		mObjectRoot->getOrCreateStateSet()->setAttribute(mRiggingProgram, osg::StateAttribute::ON);
		mObjectRoot->getOrCreateStateSet()->addUniform(mBoneMatrixArray, osg::StateAttribute::ON);

		// set all bones to identity to force bind pose
		for(size_t i = 0; i < OD_MAX_BONE_COUNT; ++i)
		{
			mBoneMatrixArray->setElement(i, osg::Matrixf::identity());
		}

		mSkeletonRoot->addUpdateCallback(mUploadCallback);
	}

	SkeletonAnimationPlayer::~SkeletonAnimationPlayer()
	{
		mObjectRoot->getOrCreateStateSet()->removeAttribute(mRiggingProgram);
		mObjectRoot->getOrCreateStateSet()->removeUniform(mBoneMatrixArray);
		mSkeletonRoot->removeUpdateCallback(mUploadCallback);
	}

	void SkeletonAnimationPlayer::setAnimation(osg::ref_ptr<Animation> anim, double startDelay)
	{
		if(anim->getModelNodeCount() != mAnimators.size())
		{
			Logger::warn() << "Animation '" << anim->getName() << "' requires different number of nodes than controlled by animator";
		}

		mCurrentAnimation = anim;

		for(auto it = mAnimators.begin(); it != mAnimators.end(); ++it)
		{
			BoneNode *bn = dynamic_cast<BoneNode*>((*it)->getNode());
        	if(bn == nullptr)
        	{
        		continue;
        	}

			int32_t jointIndex = bn->getJointInfoIndex();

			Animation::AnimStartEndPair startEnd = mCurrentAnimation->getKeyframesForNode(jointIndex);

			(*it)->setKeyframes(startEnd.first, startEnd.second, startDelay);
		}
	}

	void SkeletonAnimationPlayer::play(bool looping)
	{
		for(auto it = mAnimators.begin(); it != mAnimators.end(); ++it)
		{
			(*it)->play(looping);
		}
	}

	void SkeletonAnimationPlayer::stop()
	{
	    for(auto it = mAnimators.begin(); it != mAnimators.end(); ++it)
        {
            (*it)->stop();
        }
	}

	bool SkeletonAnimationPlayer::isPlaying()
	{
	    for(auto it = mAnimators.begin(); it != mAnimators.end(); ++it)
        {
            if((*it)->isPlaying())
            {
                return true;
            }
        }

	    return false;
	}

}
