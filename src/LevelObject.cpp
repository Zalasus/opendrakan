/*
 * Object.cpp
 *
 *  Created on: 8 Feb 2018
 *      Author: zal
 */

#include "LevelObject.h"

#include <algorithm>

#include "Level.h"
#include "Exception.h"
#include "OdDefines.h"
#include "NodeMasks.h"
#include "rfl/RflClass.h"
#include "physics/BulletAdapter.h"

#define OD_OBJECT_FLAG_VISIBLE 0x001
#define OD_OBJECT_FLAG_SCALED  0x100

namespace od
{

    class LevelObjectUpdateCallback : public osg::NodeCallback
    {
        public:

        LevelObjectUpdateCallback(LevelObject &obj)
        : mLevelObject(obj)
        , mLastSimTime(0)
        , mFirstUpdate(true)
        {
        }

        virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
        {
            const osg::FrameStamp *fs = nv->getFrameStamp();
            double simTime = 0;
            if(fs != nullptr)
            {
                if(mFirstUpdate)
                {
                    mLastSimTime = fs->getSimulationTime();
                    mFirstUpdate = false;
                }

                simTime = fs->getSimulationTime();
            }

            mLevelObject.update(simTime, simTime - mLastSimTime);

            mLastSimTime = simTime;

            traverse(node, nv);
        }

    private:

        LevelObject &mLevelObject;
        double mLastSimTime;
        bool mFirstUpdate;
    };



    LevelObject::LevelObject(Level &level)
    : mLevel(level)
    , mId(0)
    , mFlags(0)
    , mInitialEventCount(0)
    , mTransform(new osg::PositionAttitudeTransform)
    , mState(LevelObjectState::NotLoaded)
    , mSpawnStrategy(SpawnStrategy::WhenInSight)
    , mIsVisible(true)
    , mIgnoreAttachmentRotation(true)
    {
        this->setNodeMask(NodeMasks::Object);
    }

    LevelObject::~LevelObject()
    {
        // make sure we perform the despawn cleanup in case we didnt despawn before getting deleted
        if(mState == LevelObjectState::Spawned)
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

        // TODO: We could probably put this into the spawning method, along with delaying model loading of classes to when getModel() is called
        if(mClass->hasModel())
        {
            mTransform->addChild(mClass->getModel());
            this->addChild(mTransform);

            // if model defines a skeleton, create an instance of that skeleton for this object
            if(mClass->getModel()->getSkeletonBuilder() != nullptr)
            {
                mSkeletonRoot = new osg::Group;
                mClass->getModel()->getSkeletonBuilder()->build(mSkeletonRoot);
                mTransform->addChild(mSkeletonRoot);
            }
        }

        _setVisible(mFlags & OD_OBJECT_FLAG_VISIBLE);

        mRflClassInstance = mClass->makeInstance();
        if(mRflClassInstance != nullptr)
        {
            mRflClassInstance->probeFields(builder); // let builder override fields
            mRflClassInstance->loaded(mLevel.getEngine(), this);

        }else
        {
            Logger::debug() << "Could not instantiate class of level object";
        }
    }

    void LevelObject::spawned()
    {
        if(mRflClassInstance != nullptr)
        {
            mRflClassInstance->spawned(*this);
        }

        // build vector of linked object pointers from the stored indices if we haven't done that yet
        if(mLinkedObjects.size() != mLinks.size())
        {
            mLinkedObjects.reserve(mLinks.size());

            for(auto it = mLinks.begin(); it != mLinks.end(); ++it)
            {
                LevelObject &obj = mLevel.getLevelObjectByIndex(*it);
                mLinkedObjects.push_back(osg::ref_ptr<LevelObject>(&obj));
            }
        }

        Logger::debug() << "Object " << getObjectId() << " spawned";

        mState = LevelObjectState::Spawned;
    }

    void LevelObject::despawned()
    {
        if(mRflClassInstance != nullptr)
        {
            mRflClassInstance->despawned(*this);
        }

        Logger::debug() << "Object " << getObjectId() << " despawned";

        // detach this from any object it may be attached to, and detach all objects attached to this
        detach();
        _detachAllAttachedObjects();

        setEnableRflUpdateHook(false);

        mState = LevelObjectState::Loaded;
    }

    void LevelObject::destroyed()
    {
        Logger::verbose() << "Object " << getObjectId() << " destroyed";

        mState = LevelObjectState::Destroyed;
    }

    void LevelObject::update(double simTime, double relTime)
    {
        if(mState != LevelObjectState::Spawned)
        {
            return;
        }

        if(mRflClassInstance != nullptr)
        {
            mRflClassInstance->update(*this, simTime, relTime);
        }
    }

    void LevelObject::messageReceived(LevelObject &sender, odRfl::RflMessage message)
    {
        if(mState == LevelObjectState::Destroyed)
        {
            return;
        }

        Logger::verbose() << "Object " << getObjectId() << " received message '" << message << "' from " << sender.getObjectId();

        if(mRflClassInstance != nullptr)
        {
            mRflClassInstance->messageReceived(*this, sender, message);
        }
    }

    void LevelObject::setPosition(const osg::Vec3f &v)
    {
        mTransform->setPosition(v);

        for(auto it = mAttachedObjects.begin(); it != mAttachedObjects.end(); ++it)
        {
            (*it)->_attachmentTargetPositionUpdated();
        }
    }

    void LevelObject::setRotation(const osg::Quat &q)
    {
        mTransform->setAttitude(q);

        for(auto it = mAttachedObjects.begin(); it != mAttachedObjects.end(); ++it)
        {
            (*it)->_attachmentTargetPositionUpdated();
        }
    }

    void LevelObject::setVisible(bool v)
    {
        _setVisible(v);

        Logger::verbose() << "Object " << getObjectId() << " made " << (v ? "visible" : "invisible");
    }

    void LevelObject::attachTo(LevelObject *target, bool ignoreRotation, bool clearOffset)
    {
        if(target == nullptr || mAttachmentTarget != nullptr)
        {
            detach();
            return;
        }

        if(clearOffset)
        {
            setPosition(target->getPosition());
            setRotation(target->getRotation());
        }

        if(!ignoreRotation)
        {
            throw UnsupportedException("Attachment with rotation not implemented yet");
        }

        mAttachmentTarget = target;
        mIgnoreAttachmentRotation = ignoreRotation;
        mAttachmentTranslationOffset = getPosition() - target->getPosition();
        target->mAttachedObjects.push_back(osg::ref_ptr<od::LevelObject>(this));
    }

    void LevelObject::attachToChannel(LevelObject *target, size_t channelIndex, bool clearOffset)
    {
        // TODO: implement

        throw UnsupportedException("Attaching to channels not yet implemented.");
    }

    void LevelObject::detach()
    {
        if(mAttachmentTarget == nullptr)
        {
            return;
        }

        auto it = std::find(mAttachmentTarget->mAttachedObjects.begin(), mAttachmentTarget->mAttachedObjects.end(), osg::ref_ptr<od::LevelObject>(this));
        if(it != mAttachmentTarget->mAttachedObjects.end())
        {
            mAttachmentTarget->mAttachedObjects.erase(it);

        }else
        {
            Logger::warn() << "Inconsistency when detaching object: Attachment target didn't have attached object in it's list of attached objects";
        }

        mAttachmentTarget = nullptr;
    }

    void LevelObject::setEnableRflUpdateHook(bool enableHook)
    {
        if(enableHook)
        {
            if(mUpdateCallback != nullptr)
            {
                return;
            }

            mUpdateCallback = new LevelObjectUpdateCallback(*this);
            this->addUpdateCallback(mUpdateCallback);

        }else
        {
            if(mUpdateCallback == nullptr)
            {
                return;
            }

            this->removeUpdateCallback(mUpdateCallback);
            mUpdateCallback = nullptr;
        }
    }

    void LevelObject::messageAllLinkedObjects(odRfl::RflMessage message)
    {
        for(auto it = mLinkedObjects.begin(); it != mLinkedObjects.end(); ++it)
        {
            (*it)->messageReceived(*this, message);
        }
    }

    void LevelObject::requestDestruction()
    {
        mLevel.requestLevelObjectDestruction(this);
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

    void LevelObject::_attachmentTargetPositionUpdated()
    {
        if(mAttachmentTarget == nullptr)
        {
            // uhmm.... wat?
            Logger::error() << "Attachment inconsistency: Level object that was not attached to anything was asked to update to attachment target position";
            return;
        }

        osg::Vec3f newPosition;
        osg::Quat newRotation;

        if(mIgnoreAttachmentRotation)
        {
            newPosition = mAttachmentTarget->getPosition() + mAttachmentTranslationOffset;
            newRotation = getRotation();

        }else
        {
            // TODO: implement
            //newPosition = mAttachmentTarget->getPosition() + mInitialTargetRotation.inverse()*mTargetObject.getRotation()*mTranslationOffset;
            //newRotation = mTargetObject.getRotation() * mAttachedObject.getRotation();
        }

        setPosition(newPosition);
        setRotation(newRotation);
    }

    void LevelObject::_detachAllAttachedObjects()
    {
        // note: we don't call detach() on all attached objects here, as that would cause every single one of them
        //  to search itself in mAttachedObjects, unecessarily making this O(n^2)
        for(auto it = mAttachedObjects.begin(); it != mAttachedObjects.end(); ++it)
        {
            (*it)->mAttachmentTarget = nullptr;
        }

        mAttachedObjects.clear();
    }

    void LevelObject::_setVisible(bool v)
    {
        mIsVisible = v;

        if(mTransform != nullptr)
        {
            mTransform->setNodeMask(v ? NodeMasks::Object : NodeMasks::Hidden);
        }
    }

}




