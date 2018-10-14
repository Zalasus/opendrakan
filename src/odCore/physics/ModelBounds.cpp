/*
 * ModelBounds.cpp
 *
 *  Created on: 13 Apr 2018
 *      Author: zal
 */

#include <odCore/physics/ModelBounds.h>

#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>

#include <odCore/Exception.h>
#include <odCore/physics/BulletAdapter.h>

namespace od
{
    template <>
    DataReader &DataReader::operator >> <odPhysics::OrientedBoundingBox>(odPhysics::OrientedBoundingBox &obb)
    {
        osg::Vec3f bottomLeftCorner;
        (*this) >> bottomLeftCorner;

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

        osg::Vec3f dummyTranslate;
        osg::Quat  orientation;
        osg::Vec3f scale;
        osg::Quat  dummySo;
        m.decompose(dummyTranslate, orientation, scale, dummySo);

        obb.setBottomLeft(bottomLeftCorner);
        obb.setExtends(scale);
        obb.setOrientation(orientation);

        return *this;
    }
}

namespace odPhysics
{

	OrientedBoundingBox::OrientedBoundingBox()
	{
	}

	OrientedBoundingBox::OrientedBoundingBox(const OrientedBoundingBox &obb)
	: mBottomLeft(obb.mBottomLeft)
	, mExtends(obb.mExtends)
	, mOrientation(obb.mOrientation)
	{

	}

	OrientedBoundingBox &OrientedBoundingBox::operator=(const OrientedBoundingBox &obb)
	{
		mBottomLeft = obb.getMidPoint();
		mExtends = obb.getExtends();
		mOrientation = obb.getOrientation();

		return *this;
	}


	ModelBounds::ModelBounds(ModelBounds::ShapeType type, size_t shapeCount)
	: mType(type)
	, mShapeCount(shapeCount)
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
			throw od::Exception("Can't add spheres to box-based model bounds");
		}

		mSpheres.push_back(sphere);
	}

	void ModelBounds::addBox(const OrientedBoundingBox &box)
	{
		if(mType != BOXES)
		{
			throw od::Exception("Can't add boxes to sphere-based model bounds");
		}

		mBoxes.push_back(box);
	}

	btCollisionShape *ModelBounds::getCollisionShape()
	{
	    if(mCompoundShape != nullptr)
	    {
	        return mCompoundShape.get();
	    }

	    mCompoundShape.reset(new btCompoundShape(true, mShapeCount));
	    for(size_t index = 0; index < mShapeCount; ++index)
	    {
	    	size_t firstChild = mHierarchy[index].first;
			size_t nextSibling = mHierarchy[index].second;
			osg::Vec3f myTranslation;
			osg::Quat myRotation;

			// Bullet does not seem to support a manual hierarchical bounds structure. Therefore, we only care about
			//  leafs in the bounding hierarchy here and ignore all shapes that have children
			if(firstChild == 0)
			{
				std::unique_ptr<btCollisionShape> newShape;

				if(mType == SPHERES)
				{
					myTranslation = mSpheres[index].center();
					myRotation = osg::Quat(0,0,0,1);

					newShape.reset(new btSphereShape(mSpheres[index].radius()));

				}else
				{
					myTranslation = mBoxes[index].getBottomLeft() + mBoxes[index].getOrientation() * (mBoxes[index].getExtends() * 0.5);
					myRotation = mBoxes[index].getOrientation();

					btVector3 halfExtends = BulletAdapter::toBullet(mBoxes[index].getExtends() * 0.5); // btBoxShape wants half extends, so we multiply by 0.5
					newShape.reset(new btBoxShape(halfExtends));
				}

				btTransform t = BulletAdapter::makeBulletTransform(myTranslation, myRotation);
				mCompoundShape->addChildShape(t, newShape.get());
				mChildShapes.push_back(std::move(newShape));
			}
		}

	    return mCompoundShape.get();
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

			std::cout << "[] x=" << obb.getBottomLeft().x() << " y=" << obb.getBottomLeft().y() << " z=" << obb.getBottomLeft().z()
                      << " ex="<< obb.getExtends().x() << " y=" << obb.getExtends().y() << " z=" << obb.getExtends().z();

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
