/*
 * ModelBounds.h
 *
 *  Created on: 13 Apr 2018
 *      Author: zal
 */

#ifndef INCLUDE_DB_MODELBOUNDS_H_
#define INCLUDE_DB_MODELBOUNDS_H_

#include <memory>
#include <vector>
#include <osg/Matrixf>
#include <osg/BoundingSphere>
#include <BulletCollision/CollisionShapes/btCompoundShape.h>

#include "DataStream.h"

namespace od
{

	class OrientedBoundingBox
	{
	public:

		OrientedBoundingBox();
		OrientedBoundingBox(const OrientedBoundingBox &obb);

		inline osg::Vec3f getMidPoint() const { return mMidPoint; }
		inline osg::Vec3f getExtends() const { return mExtends; }
		inline osg::Quat  getOrientation() const { return mOrientation; }
		inline void setMidPoint(const osg::Vec3f &v) { mMidPoint = v; }
		inline void setExtends(const osg::Vec3f &v) { mExtends = v;}
		inline void setOrientation(const osg::Quat &q) { mOrientation = q; }

		OrientedBoundingBox &operator=(const OrientedBoundingBox &obb);


	private:

		osg::Vec3f mMidPoint;
		osg::Vec3f mExtends;
		osg::Quat  mOrientation;
	};

	/**
	 * Builder and container for bounds information of a Model. This managed the bullet objects it creates
	 * (collision shape, child shapes etc.)
	 */
	class ModelBounds
	{
	public:

		enum ShapeType { SPHERES, BOXES };

		ModelBounds(ShapeType type, size_t shapeCount);

		void setMainBounds(const osg::BoundingSpheref &sphere, const OrientedBoundingBox &box);
		void addHierarchyEntry(uint16_t firstChildIndex, uint16_t nextSiblingIndex);
		void addSphere(const osg::BoundingSpheref &sphere);
		void addBox(const OrientedBoundingBox &box);

		/// Gets or creates a collision shape to be used for Bullet dynamics. The returned pointer looses validity once
		///  the \¢ ModelBounds object is destroyed.
		btCollisionShape *getCollisionShape();

		void printInfo();


	private:

		void _recursivePrint(size_t index, size_t depth);

		ShapeType mType;
		size_t mShapeCount;
		osg::BoundingSpheref mMainSphere;
		OrientedBoundingBox mMainBox;
		std::vector<std::pair<uint16_t, uint16_t>> mHierarchy;
		std::vector<osg::BoundingSpheref> mSpheres;
		std::vector<OrientedBoundingBox> mBoxes;
		std::unique_ptr<btCompoundShape> mCompoundShape;
		std::vector<std::unique_ptr<btCollisionShape>> mChildShapes;
	};

}

#endif /* INCLUDE_DB_MODELBOUNDS_H_ */
