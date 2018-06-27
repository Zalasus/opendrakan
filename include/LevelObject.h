/*
 * Object.h
 *
 *  Created on: 8 Feb 2018
 *      Author: zal
 */

#ifndef OBJECT_H_
#define OBJECT_H_


#include <memory>
#include <list>
#include <osg/Group>
#include <osg/PositionAttitudeTransform>
#include <LinearMath/btMotionState.h>

#include "db/Class.h"
#include "anim/SkeletonAnimationPlayer.h"
#include "rfl/RflMessage.h"

namespace od
{

    class Level;

    typedef uint32_t LevelObjectId;

    enum class LevelObjectState
    {
        NotLoaded,
        Loaded,
        Spawned,
        Destroyed
    };

    enum class SpawnStrategy
    {
        Never,
        WhenInSight,
        Always
    };

    class LevelObject : public osg::Group, public btMotionState
    {
    public:

        LevelObject(Level &level);
        ~LevelObject();

        inline LevelObjectId getObjectId() const { return mId; }
        inline osg::ref_ptr<Class> getClass() { return mClass; }
        inline odRfl::RflClass *getClassInstance() { return mRflClassInstance.get(); }
        inline Level &getLevel() { return mLevel; }
        inline osg::Vec3f getPosition() const { return mTransform->getPosition(); }
        inline osg::Vec3f getScale() const { return mTransform->getScale(); }
        inline osg::Quat getRotation() const { return mTransform->getAttitude(); }
        inline Model *getModel() { return mModel; }
        inline osg::PositionAttitudeTransform *getPositionAttitudeTransform() { return mTransform; }
        inline osg::Group *getSkeletonRoot() { return mSkeletonRoot; }
        inline LevelObjectState getState() const { return mState; }
        inline void setSpawnStrategy(SpawnStrategy s) { mSpawnStrategy = s; }

        void loadFromRecord(DataReader dr);
        void spawned();
        void despawned();
        void destroyed();
        void update(double simTime, double relTime);
        void messageReceived(LevelObject &sender, odRfl::RflMessage message);

        void setPosition(const osg::Vec3f &v);
        void setRotation(const osg::Quat &q);

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
         * @brief Detaches this object from any objects. Will do nothing if this object was not attached to anything.
         */
        void detach();

        /**
         * @brief Enables or disables the RFL update hook.
         *
         * Do not disable the update callback by calling this method from the update hook itself! Doing so messes with
         * OSG's update traversal and will likely cause it to segfault.
         */
        void setEnableRflUpdateHook(bool enableHook);
        void messageAllLinkedObjects(odRfl::RflMessage message);
        void requestDestruction();

        // override osg::Group
        virtual const char *libraryName() const override { return "od";    }
        virtual const char *className()   const override { return "LevelObject"; }

        // implement btMotionState
        virtual void getWorldTransform(btTransform& worldTrans) const override;
        virtual void setWorldTransform(const btTransform& worldTrans) override;


    private:

        void _attachmentTargetPositionUpdated();
        void _detachAllAttachedObjects();


        Level &mLevel;
        LevelObjectId mId;
        AssetRef mClassRef;
        osg::ref_ptr<Class> mClass;
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
        LevelObjectState mState;
        SpawnStrategy mSpawnStrategy;

        std::vector<osg::ref_ptr<LevelObject>> mLinkedObjects;

        osg::ref_ptr<od::LevelObject> mAttachmentTarget;
        osg::Vec3f mAttachmentTranslationOffset;
        bool mIgnoreAttachmentRotation;
        std::list<osg::ref_ptr<od::LevelObject>> mAttachedObjects;

        osg::ref_ptr<osg::NodeCallback> mUpdateCallback;
    };

}



#endif /* OBJECT_H_ */
