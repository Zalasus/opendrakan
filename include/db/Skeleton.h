/*
 * Skeleton.h
 *
 *  Created on: 11 Mar 2018
 *      Author: zal
 */

#ifndef INCLUDE_DB_SKELETON_H_
#define INCLUDE_DB_SKELETON_H_

#include <string>
#include <vector>
#include <osg/Matrixf>
#include <osg/Referenced>


namespace od
{

	class Bone : public osg::Referenced
	{
	public:

		friend class Skeleton;

		Bone(const std::string &name, int32_t parentIndex);
		Bone(Bone &b) = delete;
		Bone(const Bone &b) = delete;
		~Bone() = default;

		inline Bone *getParent() { return mParent; }
		inline int32_t getParentIndex() { return mParentIndex; }
		inline std::string getName() { return mName; }

		void setParent(Bone *parent);
		void setInverseBindPoseXform(const osg::Matrixf &inverseBindPoseXform);

		void debugLogBone(size_t depth);

	private:

		std::string mName;
		osg::Matrixf mInverseBindPoseXform;
		osg::Matrixf mCurrentXform;
		Bone *mParent;
		int32_t mParentIndex;
		std::vector<Bone*> mChildren;
	};

	/**
	 * Class representing a rigged model's skeleton. Stores all bones in a flat
	 * structure. The bones themselves define the skeleton hierarchy via children and parent pointers.
	 */
	class Skeleton : public osg::Referenced
	{
	public:

		/// Constructs new Skeleton and reserves storage for \c boneCount bones
		Skeleton(size_t boneCount = 0);
		Skeleton(Skeleton&) = delete;
		Skeleton(const Skeleton&) = delete;
		~Skeleton() = default;

		Bone *addBone(const std::string &name, int32_t parentIndex);
		Bone *getBoneByIndex(size_t index);
		Bone *getRootBone();

		void buildBoneTree();

		void debugLogSkeleton();

	private:

		Bone mRootBone;
		std::vector<osg::ref_ptr<Bone>> mBones;
	};


	class BuilderBone
	{
	public:

		friend class SkeletonBuilder;

		BuilderBone(const std::string &name, int32_t parentIndex);


		inline int32_t getParentIndex() { return mParentIndex; }
		inline std::string getName() { return mName; }

		void setBoneData(osg::Matrixf &boneXform, int32_t meshIndex, int32_t firstChildIndex, int32_t nextSiblingIndex);
		void addChild(BuilderBone &b) { mChildren.push_back(&b); }
		void print(size_t depth);

	private:

		std::string mName;
		osg::Matrixf mInverseBindPoseXform;
		int32_t mParentIndex;
		int32_t mMeshIndex;
		int32_t mFirstChildIndex;
		int32_t mNextSiblingIndex;
		bool mVisited;
		std::vector<BuilderBone*> mChildren;
	};

	class SkeletonBuilder
	{
	public:

		SkeletonBuilder(const std::string &modelName);

		void reserveBones(size_t boneCount);
		void addBoneName(const std::string &name, int32_t parentIndex);
		void setBoneData(size_t boneIndex, osg::Matrixf &boneXform, int32_t meshIndex, int32_t firstChildIndex, int32_t nextSiblingIndex);
		void build();


	private:

		void _buildRecursive(BuilderBone &parent, BuilderBone &current);

		std::string mModelName;
		std::vector<BuilderBone> mBuilderBones;

	};

}

#endif /* INCLUDE_DB_SKELETON_H_ */
