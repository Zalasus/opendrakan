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
#include <ostream>
#include <osg/Matrixf>
#include <osg/Referenced>
#include <osg/NodeVisitor>
#include <osg/Group>
#include <osg/MatrixTransform>


namespace od
{

    class BoneVisitor;

    /**
     * Class representing a bone in a skeleton as a MatrixTransform. This way we can use the same
     * method to animate bones as we use to move objects around in an interpolated manner.
     */
    class BoneNode : public osg::MatrixTransform
    {
    public:

        BoneNode(const std::string &name, int32_t jointInfoIndex);

        inline int32_t getJointInfoIndex() const { return mJointInfoIndex; }

        inline bool isChannel() const { return mIsChannel; }
        inline void setIsChannel(bool b) { mIsChannel = b; }

        inline osg::Matrixf getInverseBindPoseXform() const { return mInverseBindPoseXform; }
        void setInverseBindPoseXform(const osg::Matrixf &m);


        void accept(BoneVisitor &bv);


    private:

        int32_t mJointInfoIndex;
        osg::Matrixf mInverseBindPoseXform;
        bool mIsChannel;
    };


    class BoneVisitor : public osg::NodeVisitor
    {
    public:

        BoneVisitor(osg::Uniform &boneMatrixArray);

        virtual void apply(BoneNode &b);

    };

	class SkeletonBuilder
	{
	public:

		SkeletonBuilder();

		void addBoneNode(const std::string &name, int32_t jointInfoIndex);
		void addJointInfo(osg::Matrixf &boneXform, int32_t meshIndex, int32_t firstChildIndex, int32_t nextSiblingIndex);
		void makeChannel(uint32_t jointIndex);
		void build();
		void printInfo(std::ostream &out);


	private:

		struct SkeletonJointInfo
        {
            osg::Matrixf boneXform;
            int32_t meshIndex;
            int32_t firstChildIndex;
            int32_t nextSiblingIndex;

            bool visited;
            BoneNode *referencingBone;
        };

		void _buildRecursive(BoneNode &parent, SkeletonJointInfo &current);

		std::vector<osg::ref_ptr<BoneNode>> mBoneNodes;
		std::vector<SkeletonJointInfo> mJointInfos;
		BoneNode mRootBone;
	};

}

#endif /* INCLUDE_DB_SKELETON_H_ */
