/*
 * Object.h
 *
 *  Created on: 8 Feb 2018
 *      Author: zal
 */

#ifndef OBJECT_H_
#define OBJECT_H_


#include <memory>
#include <osg/Group>
#include <osg/PositionAttitudeTransform>

#include "db/Class.h"
#include "anim/SkeletonAnimationPlayer.h"

namespace od
{

    class Level;

    typedef uint32_t LevelObjectId;

    class LevelObject : public osg::Group
    {
    public:

        LevelObject(Level &level);

        inline LevelObjectId getObjectId() const { return mId; }
        inline ClassPtr getClass() { return mClass; }
        inline Level &getLevel() { return mLevel; }
        inline osg::Vec3f getPosition() const { return mPosition; }
        inline osg::Vec3f getScale() const { return mScale; }
        inline osg::Quat getRotation() const { return mRotation; }
        void setPosition(osg::Vec3f v);
        inline SkeletonAnimationPlayer *getSkeletonAnimationPlayer() { return mSkeletonAnimation; }
        inline Model *getModel() { return mModel; }

        void loadFromRecord(DataReader dr);

        // override osg::Group
		virtual const char *libraryName() const override { return "od";    }
        virtual const char *className()   const override { return "LevelObject"; }

    private:

        Level &mLevel;
        LevelObjectId mId;
        AssetRef mClassRef;
        ClassPtr mClass;
        std::unique_ptr<RflClass> mRflClassInstance;
        osg::Vec3f mPosition;
        uint32_t mFlags;
        uint16_t mInitialEventCount;
        std::vector<uint16_t> mLinks;
        osg::Vec3f mScale;
        osg::Quat  mRotation;

        osg::ref_ptr<osg::PositionAttitudeTransform> mTransform;
        osg::ref_ptr<Model> mModel;
        osg::ref_ptr<osg::Group> mSkeletonRoot;
        osg::ref_ptr<SkeletonAnimationPlayer> mSkeletonAnimation;
    };

    typedef osg::ref_ptr<od::LevelObject> LevelObjectPtr;
}



#endif /* OBJECT_H_ */
