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

#include <odCore/DataStream.h>

namespace odPhysics
{

	class OrientedBoundingBox
	{
	public:

		OrientedBoundingBox();
		OrientedBoundingBox(const OrientedBoundingBox &obb);

		inline osg::Vec3f getBottomLeft() const { return mBottomLeft; }
		inline osg::Vec3f getMidPoint() const { return mBottomLeft + mExtends * 0.5; }
		inline osg::Vec3f getExtends() const { return mExtends; }
		inline osg::Quat  getOrientation() const { return mOrientation; }
		inline void setBottomLeft(const osg::Vec3f &v) { mBottomLeft = v; }
		inline void setExtends(const osg::Vec3f &v) { mExtends = v;}
		inline void setOrientation(const osg::Quat &q) { mOrientation = q; }

		OrientedBoundingBox &operator=(const OrientedBoundingBox &obb);


	private:

		osg::Vec3f mBottomLeft;
		osg::Vec3f mExtends;
		osg::Quat  mOrientation;
	};

	class ModelCollisionShape : public osg::Referenced, public btCompoundShape
    {
    public:

	    ModelCollisionShape(size_t initialChildShapeCapacity);

	    void addManagedChildShape(btTransform xform, btCollisionShape *shape);


    private:

	    using btCompoundShape::addChildShape;

	    std::vector<std::unique_ptr<btCollisionShape>> mChildShapes;
    };

	/**
	 * Builder and container for bounds information of a Model. This manages the bullet objects it creates
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

		/**
		 * Builds or returns a shape that can be shared among all non-scaled uses of the model.
		 */
		ModelCollisionShape *getSharedCollisionShape();

		/**
		 * Allocates and builds a new shape, even if one is already cached in this bounds object.
		 *
		 * @note This transfers ownership. The returned pointer must be stored in an osg::ref_ptr
		 */
		ModelCollisionShape *buildNewShape();

		void printInfo();


	private:

		void _recursivePrint(size_t index, size_t depth);
		void _recursiveBuild(osg::Vec3f parentTranslation, osg::Quat parentRotation, size_t index, size_t depth);

		ShapeType mType;
		size_t mShapeCount;
		osg::BoundingSpheref mMainSphere;
		OrientedBoundingBox mMainBox;
		std::vector<std::pair<uint16_t, uint16_t>> mHierarchy;
		std::vector<osg::BoundingSpheref> mSpheres;
		std::vector<OrientedBoundingBox> mBoxes;
		osg::ref_ptr<ModelCollisionShape> mSharedShape;
	};

}

#endif /* INCLUDE_DB_MODELBOUNDS_H_ */
