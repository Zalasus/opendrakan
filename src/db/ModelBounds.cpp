/*
 * ModelBounds.cpp
 *
 *  Created on: 13 Apr 2018
 *      Author: zal
 */

#include "db/ModelBounds.h"

#include "Exception.h"

namespace od
{

	OrientedBoundingBox::OrientedBoundingBox()
	{
	}

	OrientedBoundingBox::OrientedBoundingBox(const OrientedBoundingBox &obb)
	: mMidPoint(obb.mMidPoint)
	, mTransform(obb.mTransform)
	{
	}

	OrientedBoundingBox &OrientedBoundingBox::operator=(const OrientedBoundingBox &obb)
	{
		mMidPoint = obb.getMidPoint();
		mTransform = obb.getTransform();

		return *this;
	}

	template <>
	DataReader &DataReader::operator >> <OrientedBoundingBox>(OrientedBoundingBox &obb)
	{
	    osg::Vec3f midpoint;
	    (*this) >> midpoint;

	    float t[9];
	    osg::Matrixf m; // don't read transform matrix directly, as the deserializer for osg::Matrix reads a 3x4 matrix!!!
	    for(size_t i = 0; i < sizeof(t)/sizeof(float); ++i)
	    {
	    	(*this) >> t[i];
	    }

	    m.set(t[0], t[1], t[2], 0,
	    	  t[3], t[4], t[5], 0,
			  t[6], t[7], t[8], 0,
			     0,    0,    0, 1);

	    obb.setMidPoint(midpoint);
	    obb.setTransform(m);

	    return *this;
	}


	ModelBounds::ModelBounds(ModelBounds::ShapeType type, size_t shapeCount)
	: mType(type)
	{
		mHierarchy.reserve(shapeCount);

		if(mType == BOXES)
		{
			mBoxes.reserve(shapeCount);

		}else
		{
			mSpheres.reserve(shapeCount);
		}
	}

	void ModelBounds::setMainBounds(const osg::BoundingSpheref &sphere, const OrientedBoundingBox &box)
	{
		mMainSphere = sphere;
		mMainBox = box;
	}

	void  ModelBounds::addHierarchyEntry(uint16_t firstChildIndex, uint16_t nextSiblingIndex)
	{
		mHierarchy.push_back(std::make_pair(firstChildIndex, nextSiblingIndex));
	}

	void ModelBounds::addSphere(const osg::BoundingSpheref &sphere)
	{
		if(mType != SPHERES)
		{
			throw Exception("Can't add spheres to box-based model bounds");
		}

		mSpheres.push_back(sphere);
	}

	void ModelBounds::addBox(const OrientedBoundingBox &box)
	{
		if(mType != BOXES)
		{
			throw Exception("Can't add boxes to sphere-based model bounds");
		}

		mBoxes.push_back(box);
	}

	void ModelBounds::printInfo()
	{
		_recursivePrint(0,0);
	}

	void ModelBounds::_recursivePrint(size_t index, size_t depth)
	{
		if(index >= mHierarchy.size())
		{
			return;
		}

		for(size_t i = 0; i < depth; ++i)
		{
			std::cout << "   |";
		}

		if(depth > 0)
		{
			std::cout << "-";
		}

		if(mType == BOXES)
		{
			OrientedBoundingBox &obb = mBoxes[index];

			std::cout << "[] x=" << obb.getMidPoint().x() << " y=" << obb.getMidPoint().y() << " z=" << obb.getMidPoint().z();

		}else
		{
			osg::BoundingSpheref &bs = mSpheres[index];

			std::cout << "()" << " r=" << bs.radius() << " x=" << bs.center().x() << " y=" << bs.center().y() << " z=" << bs.center().z();
		}

		std::cout << std::endl;

		size_t firstChild = mHierarchy[index].first;
		if(firstChild != 0)
			_recursivePrint(firstChild, depth+1);

		size_t nextSibling = mHierarchy[index].second;
		if(nextSibling != 0)
			_recursivePrint(nextSibling, depth);
	}

}
