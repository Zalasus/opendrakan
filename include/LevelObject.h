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
#include <LinearMath/btMotionState.h>

#include "db/Class.h"
#include "anim/SkeletonAnimationPlayer.h"
#include "anim/MotionAnimator.h"

namespace od
{

    class Level;

    typedef uint32_t LevelObjectId;

    class LevelObject : public osg::Group, public btMotionState
    {
    public:

        LevelObject(Level &level);

        inline LevelObjectId getObjectId() const { return mId; }
        inline ClassPtr getClass() { return mClass; }
        inline odRfl::RflClass *getClassInstance() { return mRflClassInstance.get(); }
        inline Level &getLevel() { return mLevel; }
        inline osg::Vec3f getPosition() const { return mTransform->getPosition(); }
        inline osg::Vec3f getScale() const { return mTransform->getScale(); }
        inline osg::Quat getRotation() const { return mTransform->getAttitude(); }
        inline void setPosition(osg::Vec3f v) { mTransform->setPosition(v); }
        inline void setRotation(osg::Quat q) { mTransform->setAttitude(q); }
        inline SkeletonAnimationPlayer *getSkeletonAnimationPlayer() { return mSkeletonAnimation; }
        inline Model *getModel() { return mModel; }

        void loadFromRecord(DataReader dr);
        MotionAnimator *getOrCreateMotionAnim();

        // override osg::Group
		virtual const char *libraryName() const override { return "od";    }
        virtual const char *className()   const override { return "LevelObject"; }

        // implement btMotionState
        virtual void getWorldTransform(btTransform& worldTrans) const override;
		virtual void setWorldTransform(const btTransform& worldTrans) override;


    private:

        Level &mLevel;
        LevelObjectId mId;
        AssetRef mClassRef;
        ClassPtr mClass;
        std::unique_ptr<odRfl::RflClass> mRflClassInstance;
        osg::Vec3f mInitialPosition;
        uint32_t mFlags;
        uint16_t mInitialEventCount;
        std::vector<uint16_t> mLinks;
        osg::Vec3f mInitialScale;
        osg::Quat  mInitialRotation;

        osg::ref_ptr<osg::PositionAttitudeTransform> mTransform;
        osg::ref_ptr<Model> mModel;
        osg::ref_ptr<osg::Group> mSkeletonRoot;
        osg::ref_ptr<SkeletonAnimationPlayer> mSkeletonAnimation;
        osg::ref_ptr<MotionAnimator> mMotionAnimator;
    };

    typedef osg::ref_ptr<od::LevelObject> LevelObjectPtr;
}



#endif /* OBJECT_H_ */
