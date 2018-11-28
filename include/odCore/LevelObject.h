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
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <LinearMath/btMotionState.h>

#include <odCore/db/Class.h>
#include <odCore/rfl/RflMessage.h>

namespace odRender
{
    class ObjectNode;
}

namespace od
{

    class Level;
    class Layer;

    typedef uint32_t LevelObjectId;

    enum class LevelObjectState
    {
        NotLoaded,
        Loaded,
        Spawned,
        Destroyed
    };

    enum class LevelObjectType
    {
        Normal,
        Light,
        Detector
    };

    enum class SpawnStrategy
    {
        Never,
        WhenInSight,
        Always
    };

    class LevelObject : public btMotionState
    {
    public:

        LevelObject(Level &level);
        ~LevelObject();

        inline LevelObjectId getObjectId() const { return mId; }
        inline odDb::Class *getClass() { return mClass; }
        inline odRfl::RflClass *getClassInstance() { return mRflClassInstance.get(); }
        inline Level &getLevel() { return mLevel; }
        inline glm::vec3 getPosition() const { return mPosition; }
        inline glm::vec3 getScale() const { return mScale; }
        inline glm::quat getRotation() const { return mRotation; }
        inline LevelObjectState getState() const { return mState; }
        inline LevelObjectType getObjectType() const { return mObjectType; }
        inline void setSpawnStrategy(SpawnStrategy s) { mSpawnStrategy = s; }
        inline const std::vector<LevelObject*> &getLinkedObjects() const { return mLinkedObjects; }
        inline Layer *getLightSourceLayer() { return mLightingLayer; }
        inline bool isVisible() const { return mIsVisible; }
        inline bool isScaled() const { return mIsScaled; }

        void loadFromRecord(DataReader dr);
        void spawned();
        void despawned();
        void destroyed();
        void update(float relTime);
        void messageReceived(LevelObject &sender, odRfl::RflMessage message);

        void setPosition(const glm::vec3 &v);
        void setRotation(const glm::quat &q);
        void setScale(const glm::vec3 &scale);
        void setVisible(bool v);

        void setObjectType(LevelObjectType type);

        /**
         * @brief Attaches this object to target object.
         *
         * Attaches this LevelObject to the LevelObject \c target. Any transformation applied to \c target will also be applied to
         * \c this. This only happens in a relative context. For instance, any offset between this and \c target will be maintained
         * unless \c ignoreTranslation is true and the scale ration between these two object will be maintained unless
         * \c ignoreScale is true.
         *
         * If \c ignoreTranslation is true, translation changes applied to \c target will not be transferred to this object.
         * If \c ignoreRotation is true, rotation  changesapplied to \c target will not be transferred to this object.
         * If \c ignoreScale is true, scaling changes applied to \c target will not be transferred to this object.
         *
         * If \c ignoreTranslation, \c ignoreRotation and \c ignoreScale are all true, this method will throw.
         *
         * Calling this with \c target as \c nullptr is equivalent to calling \c detach().
         *
         * If this is called on a LevelObject that is already attached to something, it will be detached first.
         *
         * If the attachment target despawns, this object will automatically be detached.
         */
        void attachTo(LevelObject *target, bool ignoreTranslation, bool ignoreRotation, bool ignoreScale);

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
         */
        void setEnableRflUpdateHook(bool enableHook);
        void messageAllLinkedObjects(odRfl::RflMessage message);
        void requestDestruction();

        // implement btMotionState
        virtual void getWorldTransform(btTransform& worldTrans) const override;
        virtual void setWorldTransform(const btTransform& worldTrans) override;


    private:

        void _onTransformChanged(LevelObject *transformChangeSource);
        void _updateLayerBelowObject();
        void _attachmentTargetsTransformUpdated(LevelObject *transformChangeSource); // pass along source so we can detect circular attachments
        void _detachAllAttachedObjects();
        void _setRenderNodeVisible(bool visible);


        Level &mLevel;
        LevelObjectId mId;
        odDb::AssetRef mClassRef;
        odDb::AssetPtr<odDb::Class> mClass;
        std::unique_ptr<odRfl::RflClass> mRflClassInstance;
        uint32_t mLightingLayerId;
        Layer *mLightingLayer;
        glm::vec3 mInitialPosition;
        uint32_t mFlags;
        uint16_t mInitialEventCount;
        std::vector<uint16_t> mLinks;
        glm::vec3 mInitialScale;
        glm::quat mInitialRotation;
        bool mIsScaled;

        glm::vec3 mPosition;
        glm::quat mRotation;
        glm::vec3 mScale;

        LevelObjectState mState;
        LevelObjectType mObjectType;
        SpawnStrategy mSpawnStrategy;
        bool mIsVisible;

        std::vector<LevelObject*> mLinkedObjects;

        od::LevelObject *mAttachmentTarget;
        std::list<od::LevelObject*> mAttachedObjects;
        glm::vec3 mAttachmentTranslationOffset;
        glm::quat mAttachmentRotationOffset;
        glm::vec3 mAttachmentScaleRatio;
        bool mIgnoreAttachmentTranslation;
        bool mIgnoreAttachmentRotation;
        bool mIgnoreAttachmentScale;

        bool mLayerBelowObjectDirty;
        Layer *mLayerBelowObject;

        bool mRflUpdateHookEnabled;

        od::RefPtr<odRender::ObjectNode> mRenderNode;
    };

}



#endif /* OBJECT_H_ */
