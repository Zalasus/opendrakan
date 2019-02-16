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

#include <odCore/DataStream.h>
#include <odCore/BoundingBox.h>
#include <odCore/BoundingSphere.h>

namespace odDb
{

	/**
	 * Container for bounds information of a Model.
	 */
	class ModelBounds
	{
	public:

		enum ShapeType { SPHERES, BOXES };

		ModelBounds(ShapeType type, size_t shapeCount);

		inline const od::BoundingSphere &getMainSphere() const { return mMainSphere; }
		inline const od::OrientedBoundingBox &getMainBox() const { return mMainBox; }
		inline size_t getShapeCount() const { return mShapeCount; }
		inline ShapeType getShapeType() const { return mType; }
		inline const std::vector<std::pair<uint16_t, uint16_t>> &getHierarchy() const { return mHierarchy; }
		inline const std::vector<od::BoundingSphere> &getSpheres() const { return mSpheres; }
		inline const std::vector<od::OrientedBoundingBox> &getBoxes() const { return mBoxes; }

		void setMainBounds(const od::BoundingSphere &sphere, const od::OrientedBoundingBox &box);
		void addHierarchyEntry(uint16_t firstChildIndex, uint16_t nextSiblingIndex);
		void addSphere(const od::BoundingSphere &sphere);
		void addBox(const od::OrientedBoundingBox &box);

		void printInfo();


	private:

		void _recursivePrint(size_t index, size_t depth);

		ShapeType mType;
		size_t mShapeCount;
		od::BoundingSphere mMainSphere;
		od::OrientedBoundingBox mMainBox;
		std::vector<std::pair<uint16_t, uint16_t>> mHierarchy;
		std::vector<od::BoundingSphere> mSpheres;
		std::vector<od::OrientedBoundingBox> mBoxes;
	};

}

#endif /* INCLUDE_DB_MODELBOUNDS_H_ */
