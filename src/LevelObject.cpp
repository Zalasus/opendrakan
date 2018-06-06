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
#include "physics/BulletAdapter.h"

#define OD_OBJECT_FLAG_VISIBLE 0x001
#define OD_OBJECT_FLAG_SCALED  0x100

namespace od
{

    class AttachmentUpdateCallback : public osg::NodeCallback
    {
    public:

        AttachmentUpdateCallback(LevelObject &attachedObject, LevelObject &targetObject, bool ignoreRotation)
        : mAttachedObject(attachedObject)
        , mTargetObject(targetObject)
        , mIgnoreRotation(ignoreRotation)
        {
            mTranslationOffset = (mAttachedObject.getPosition() - mTargetObject.getPosition());
            mInitialAttachedRotation = mAttachedObject.getRotation();
            mInitialTargetRotation = mTargetObject.getRotation();
        }

        LevelObject &getAttachedObject()
        {
            return mAttachedObject;
        }

        LevelObject &getTargetObject()
        {
            return mTargetObject;
        }

        virtual void operator()(osg::Node *node, osg::NodeVisitor *nv)
        {
            // traverse first, so any updates to the target object's position are applied before updating ours
            traverse(node, nv);

            osg::Vec3f newPosition;
            osg::Quat newRotation;

            if(!mIgnoreRotation)
            {
                newPosition = mTargetObject.getPosition() + mInitialTargetRotation.inverse()*mTargetObject.getRotation()*mTranslationOffset;
                newRotation = mTargetObject.getRotation() * mAttachedObject.getRotation();

            }else
            {
                newPosition = mTargetObject.getPosition() + mTranslationOffset;
                newRotation = mAttachedObject.getRotation();
            }

            mAttachedObject.setPosition(newPosition);
            mAttachedObject.setRotation(newRotation);
        }


    private:

        LevelObject &mAttachedObject;
        LevelObject &mTargetObject;
        bool mIgnoreRotation;
        osg::Vec3f mTranslationOffset;
        osg::Quat mInitialAttachedRotation;
        osg::Quat mInitialTargetRotation;
    };




    LevelObject::LevelObject(Level &level)
    : mLevel(level)
    , mId(0)
    , mFlags(0)
    , mInitialEventCount(0)
    , mTransform(new osg::PositionAttitudeTransform)
    , mSpawned(false)
    {
    }

    LevelObject::~LevelObject()
    {
        // make sure we perform the despawn cleanup in case we didnt despawn before getting deleted
        if(mSpawned)
        {
            Logger::warn() << "Level object deleted while still spawned";

            despawned();
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
            mRflClassInstance->loaded(mLevel.getEngine(), this);

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

            // if model defines a skeleton, create an instance of that skeleton for this object
            if(mClass->getModel()->getSkeletonBuilder() != nullptr)
            {
                mSkeletonRoot = new osg::Group;
                mClass->getModel()->getSkeletonBuilder()->build(mSkeletonRoot);
                mTransform->addChild(mSkeletonRoot);
            }
        }
    }

    void LevelObject::spawned()
    {
        if(mRflClassInstance != nullptr)
        {
            mRflClassInstance->spawned(*this);
        }

        Logger::debug() << "Object " << getObjectId() << " spawned";

        mSpawned = true;
    }

    void LevelObject::despawned()
    {
        if(mRflClassInstance != nullptr)
        {
            mRflClassInstance->despawned(*this);
        }

        Logger::debug() << "Object " << getObjectId() << " despawned";

        mSpawned = false;
    }

    void LevelObject::attachTo(LevelObject *target, bool ignoreRotation, bool clearOffset)
    {
        if(target == nullptr)
        {
            detach();
            return;
        }

        if(clearOffset)
        {
            setPosition(target->getPosition());
            setRotation(target->getRotation());
        }

        mAttachmentCallback = new AttachmentUpdateCallback(*this, *target, ignoreRotation);
        target->addUpdateCallback(mAttachmentCallback);
    }

    void LevelObject::attachToChannel(LevelObject *target, size_t channelIndex, bool clearOffset)
    {
        // TODO: implement

        throw UnsupportedException("Attaching to channels not yet implemented.");
    }

    LevelObject *LevelObject::detach()
    {
        if(mAttachmentCallback == nullptr)
        {
            return nullptr;
        }

        AttachmentUpdateCallback *callback = static_cast<AttachmentUpdateCallback*>(mAttachmentCallback.get());
        LevelObject *target = &callback->getTargetObject();

        // FIXME: will segfault if target object despawned prior to detaching
        target->removeUpdateCallback(callback);

        return target;
    }

    void LevelObject::getWorldTransform(btTransform& worldTrans) const
    {
        worldTrans = BulletAdapter::makeBulletTransform(getPosition(), getRotation());
    }

    void LevelObject::setWorldTransform(const btTransform& worldTrans)
    {
        setPosition(BulletAdapter::toOsg(worldTrans.getOrigin()));
        setRotation(BulletAdapter::toOsg(worldTrans.getRotation()));
    }

}




