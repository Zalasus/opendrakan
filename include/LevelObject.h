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

namespace od
{

    class Level;

    typedef uint32_t LevelObjectId;

    class LevelObject : public osg::Group, public btMotionState
    {
    public:

        LevelObject(Level &level);
        ~LevelObject();

        inline LevelObjectId getObjectId() const { return mId; }
        inline ClassPtr getClass() { return mClass; }
        inline odRfl::RflClass *getClassInstance() { return mRflClassInstance.get(); }
        inline Level &getLevel() { return mLevel; }
        inline osg::Vec3f getPosition() const { return mTransform->getPosition(); }
        inline osg::Vec3f getScale() const { return mTransform->getScale(); }
        inline osg::Quat getRotation() const { return mTransform->getAttitude(); }
        inline void setPosition(osg::Vec3f v) { mTransform->setPosition(v); }
        inline void setRotation(osg::Quat q) { mTransform->setAttitude(q); }
        inline Model *getModel() { return mModel; }
        inline osg::PositionAttitudeTransform *getPositionAttitudeTransform() { return mTransform; }
        inline osg::Group *getSkeletonRoot() { return mSkeletonRoot; }

        void loadFromRecord(DataReader dr);
        void spawned();
        void despawned();

        /**
         * @brief Attaches this object to target object.
         *
         * Attaches this LevelObject to the LevelObject \c target. Any transformation applied to \c target will also be applied to
         * \c this. If \c ignoreRotation is false, any rotations transferred from \c target to this will pivot relative to \c target's origin.
         *
         * If \c clearOffset is true, \c this will be moved to \c target prior to attachment so their transformation will be kept equal
         * for the duration of the attachment.
         *
         * Calling this with \c target as \c nullptr is equivalent to calling \c detach().
         *
         * If this is called on a LevelObject that is already attached to something, it will be detached first.
         *
         * If the attachment target despawns, this object will automatically be detached.
         */
        void attachTo(LevelObject *target, bool ignoreRotation = false, bool clearOffset = false);

        /**
         * @brief Attaches this object to bone/channel of target character object.
         *
         * Attaches this LevelObject to the channel with index \c channelIndex in LevelObject \c target.
         * Any transformation applied to the respective channel in \c target will also be applied to
         * \c this. Any rotations transferred from the channel to this will pivot relative to the channel's origin.
         * If \c target has no skeleton or no channel with the given index, this method will throw.
         *
         * If \c clearOffset is true, \c this will be moved to target channel prior to attachment so their transformation will be kept equal
         * for the duration of the attachment.
         *
         * If this is called on a LevelObject that is already attached to something, it will be detached first.
         */
        void attachToChannel(LevelObject *target, size_t channelIndex, bool clearOffset = false);

        /**
         * @brief Detaches this object from any objects. Returns object it was previously attached to or \c nullptr if it wasn't attached.
         */
        LevelObject *detach();

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
        bool mSpawned;
        osg::ref_ptr<osg::NodeCallback> mAttachmentCallback;
    };

    typedef osg::ref_ptr<od::LevelObject> LevelObjectPtr;
}



#endif /* OBJECT_H_ */
