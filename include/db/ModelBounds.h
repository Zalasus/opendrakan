/*
 * ModelBounds.h
 *
 *  Created on: 13 Apr 2018
 *      Author: zal
 */

#ifndef INCLUDE_DB_MODELBOUNDS_H_
#define INCLUDE_DB_MODELBOUNDS_H_

#include <osg/Matrixf>
#include <osg/BoundingSphere>

#include "DataStream.h"

namespace od
{

	class OrientedBoundingBox
	{
	public:

		OrientedBoundingBox();
		OrientedBoundingBox(const OrientedBoundingBox &obb);

		inline void setMidPoint(const osg::Vec3f &v) { mMidPoint = v; }
		inline void setTransform(const osg::Matrixf &m) { mTransform = m; }
		inline osg::Vec3f getMidPoint() const { return mMidPoint; }
		inline osg::Matrixf getTransform() const { return mTransform; }

		OrientedBoundingBox &operator=(const OrientedBoundingBox &obb);

	private:

		osg::Vec3f mMidPoint;
		osg::Matrixf mTransform;
	};

	class ModelBounds
	{
	public:

		enum ShapeType { SPHERES, BOXES };

		ModelBounds(ShapeType type, size_t shapeCount);

		void setMainBounds(const osg::BoundingSpheref &sphere, const OrientedBoundingBox &box);
		void addHierarchyEntry(uint16_t firstChildIndex, uint16_t nextSiblingIndex);
		void addSphere(const osg::BoundingSpheref &sphere);
		void addBox(const OrientedBoundingBox &box);

		void printInfo();


	private:

		void _recursivePrint(size_t index, size_t depth);

		ShapeType mType;
		osg::BoundingSpheref mMainSphere;
		OrientedBoundingBox mMainBox;
		std::vector<std::pair<uint16_t, uint16_t>> mHierarchy;
		std::vector<osg::BoundingSpheref> mSpheres;
		std::vector<OrientedBoundingBox> mBoxes;
	};

}

#endif /* INCLUDE_DB_MODELBOUNDS_H_ */
