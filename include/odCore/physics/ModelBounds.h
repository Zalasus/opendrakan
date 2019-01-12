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

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <BulletCollision/CollisionShapes/btCompoundShape.h>

#include <odCore/DataStream.h>
#include <odCore/BoundingBox.h>
#include <odCore/BoundingSphere.h>

namespace odPhysics
{

	class ModelCollisionShape : public btCompoundShape
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

		inline const od::BoundingSphere &getMainSphere() const { return mMainSphere; }
		inline const od::OrientedBoundingBox &getMainBox() const { return mMainBox; }

		void setMainBounds(const od::BoundingSphere &sphere, const od::OrientedBoundingBox &box);
		void addHierarchyEntry(uint16_t firstChildIndex, uint16_t nextSiblingIndex);
		void addSphere(const od::BoundingSphere &sphere);
		void addBox(const od::OrientedBoundingBox &box);

		/**
		 * Builds or returns a shape that can be shared among all non-scaled uses of the model.
		 */
		std::shared_ptr<ModelCollisionShape> getSharedCollisionShape();

		/**
		 * Allocates and builds a new shape, even if one is already cached in this bounds object.
		 *
		 * @note This transfers ownership. The returned pointer must be managed by the caller.
		 */
		ModelCollisionShape *buildNewShape();

		void printInfo();


	private:

		void _recursivePrint(size_t index, size_t depth);
		void _recursiveBuild(glm::vec3 parentTranslation, glm::quat parentRotation, size_t index, size_t depth);

		ShapeType mType;
		size_t mShapeCount;
		od::BoundingSphere mMainSphere;
		od::OrientedBoundingBox mMainBox;
		std::vector<std::pair<uint16_t, uint16_t>> mHierarchy;
		std::vector<od::BoundingSphere> mSpheres;
		std::vector<od::OrientedBoundingBox> mBoxes;
		std::shared_ptr<ModelCollisionShape> mSharedShape;
	};

}

#endif /* INCLUDE_DB_MODELBOUNDS_H_ */
