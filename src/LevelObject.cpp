/*
 * Object.cpp
 *
 *  Created on: 8 Feb 2018
 *      Author: zal
 */

#include "LevelObject.h"

#include "Level.h"
#include "Exception.h"
#include "OdDefines.h"
#include "rfl/RflClass.h"

#define OD_OBJECT_FLAG_VISIBLE 0x001
#define OD_OBJECT_FLAG_SCALED  0x100

namespace od
{

    LevelObject::LevelObject(Level &level)
    : mLevel(level)
    , mId(0)
    , mFlags(0)
    , mInitialEventCount(0)
    , mTransform(new osg::PositionAttitudeTransform)
    {
    }

    void LevelObject::setPosition(osg::Vec3f v)
    {
    	if(mTransform != nullptr)
    	{
    		mTransform->setPosition(v);
    	}
    }

    void LevelObject::loadFromRecord(DataReader dr)
    {
        uint16_t xRot;
        uint16_t yRot;
        uint16_t zRot;
        uint16_t linkCount;

        dr >> mId
           >> mClassRef
           >> DataReader::Ignore(4)
           >> mInitialPosition
           >> mFlags
           >> mInitialEventCount
		   >> linkCount;

        // TODO: finally write a deserializer for vectors of things!
        mLinks.resize(linkCount);
        for(size_t i = 0; i < linkCount; ++i)
        {
        	dr >> mLinks[i];
        }

        dr >> xRot
           >> yRot
           >> zRot;

        if(mFlags & OD_OBJECT_FLAG_SCALED)
        {
            dr >> mInitialScale;

        }else
        {
        	mInitialScale.set(1,1,1);
        }

        odRfl::RflClassBuilder builder;
        builder.readFieldRecord(dr, true);

        mInitialPosition *= OD_WORLD_SCALE; // correct editor scaling
        mInitialRotation = osg::Quat(
				osg::DegreesToRadians((float)xRot), osg::Vec3(1,0,0),
				osg::DegreesToRadians((float)yRot-90), osg::Vec3(0,1,0),  // -90 deg. determined to be correct through experiment
				osg::DegreesToRadians((float)zRot), osg::Vec3(0,0,1));

        mTransform->setAttitude(mInitialRotation);
		mTransform->setPosition(mInitialPosition);
		mTransform->setScale(mInitialScale);

        mClass = mLevel.getClassByRef(mClassRef);

        mRflClassInstance = mClass->makeInstance();
        if(mRflClassInstance != nullptr)
        {
            mRflClassInstance->probeFields(builder); // let builder override fields
            mRflClassInstance->spawn(*this);

        }else
        {
        	Logger::debug() << "Could not instantiate class of level object";
        }

        // FIXME: the visible flag is just the initial state. it can be toggled in-game, so we should load the model regardless of this flag
        if(mClass->hasModel() && (mFlags & OD_OBJECT_FLAG_VISIBLE))
        {
        	mModel = mClass->getModel();
			mTransform->addChild(mModel);
			this->addChild(mTransform);

			if(mClass->getModel()->getSkeletonBuilder() != nullptr)
			{
				mSkeletonRoot = new osg::Group;
				mClass->getModel()->getSkeletonBuilder()->build(mSkeletonRoot);
				mTransform->addChild(mSkeletonRoot);

				mSkeletonAnimation = new SkeletonAnimationPlayer(mLevel.getEngine(), this, mSkeletonRoot);
			}
        }
    }

    MotionAnimator *LevelObject::getOrCreateMotionAnim()
    {
    	if(mMotionAnimator == nullptr)
    	{
    		mMotionAnimator = new MotionAnimator(mTransform);
    	}

    	return mMotionAnimator;
    }

}



