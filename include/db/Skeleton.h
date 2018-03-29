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

    /**
     * Class representing a bone in a skeleton as a MatrixTransform. This way we can use the same
     * method to animate bones as we use to move objects around in an interpolated manner.
     */
    class BoneNode : public osg::MatrixTransform
    {
    public:

    	BoneNode();
        BoneNode(const std::string &name, int32_t jointInfoIndex);
        BoneNode(const BoneNode &bn, const osg::CopyOp &copyop = osg::CopyOp::SHALLOW_COPY);

        META_Node(od, BoneNode);

        inline int32_t getJointInfoIndex() const { return mJointInfoIndex; }

        inline bool isChannel() const { return mIsChannel; }
        inline void setIsChannel(bool b) { mIsChannel = b; }

        inline osg::Matrixf getInverseBindPoseXform() const { return mInverseBindPoseXform; }
        void setInverseBindPoseXform(const osg::Matrixf &m);


    private:

        int32_t mJointInfoIndex;
        osg::Matrixf mInverseBindPoseXform;
        bool mIsChannel;
    };

	class SkeletonBuilder
	{
	public:

		SkeletonBuilder();

		void addBoneNode(const std::string &name, int32_t jointInfoIndex);
		void addJointInfo(osg::Matrixf &boneXform, int32_t meshIndex, int32_t firstChildIndex, int32_t nextSiblingIndex);
		void makeChannel(uint32_t jointIndex);
		void build(osg::Group *rootNode);
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

		void _rebuildJointLinks();
		void _buildRecursive(osg::Group &parent, SkeletonJointInfo &current);

		std::vector<osg::ref_ptr<BoneNode>> mBoneNodes;
		std::vector<SkeletonJointInfo> mJointInfos;
		bool mJointLinksDirty;
	};

}

#endif /* INCLUDE_DB_SKELETON_H_ */
