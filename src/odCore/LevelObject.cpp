/*
 * Object.cpp
 *
 *  Created on: 8 Feb 2018
 *      Author: zal
 */

#include <odCore/LevelObject.h>

#include <algorithm>

#include <odCore/Engine.h>
#include <odCore/Level.h>
#include <odCore/Layer.h>
#include <odCore/Exception.h>
#include <odCore/OdDefines.h>
#include <odCore/LightReceiver.h>

#include <odCore/anim/Skeleton.h>

#include <odCore/db/Model.h>
#include <odCore/db/ModelBounds.h>
#include <odCore/db/SkeletonBuilder.h>

#include <odCore/rfl/RflClass.h>
#include <odCore/rfl/ObjectBuilderProbe.h>

#include <odCore/render/Renderer.h>
#include <odCore/render/ObjectNode.h>
#include <odCore/render/ModelNode.h>

#include <odCore/physics/PhysicsSystem.h>
#include <odCore/physics/Handles.h>

#define OD_OBJECT_FLAG_VISIBLE 0x001
#define OD_OBJECT_FLAG_SCALED  0x100

namespace od
{

    LevelObject::LevelObject(Level &level)
    : mLevel(level)
    , mId(0)
    , mLightingLayerId(0)
    , mLightingLayer(nullptr)
    , mFlags(0)
    , mInitialEventCount(0)
    , mIsScaled(false)
    , mState(LevelObjectState::NotLoaded)
    , mObjectType(LevelObjectType::Normal)
    , mSpawnStrategy(SpawnStrategy::WhenInSight)
    , mIsVisible(true)
    , mAttachmentTarget(nullptr)
    , mIgnoreAttachmentTranslation(false)
    , mIgnoreAttachmentRotation(false)
    , mIgnoreAttachmentScale(false)
    , mLayerBelowObjectDirty(true)
    , mLayerBelowObject(nullptr)
    , mRflUpdateHookEnabled(false)
    {
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
           >> mLightingLayerId
           >> mInitialPosition
           >> mFlags
           >> mInitialEventCount
           >> linkCount;

        if(mLightingLayerId != 0)
        {
            mLightingLayer = mLevel.getLayerById(mLightingLayerId);
            if(mLightingLayer == nullptr)
            {
                Logger::error() << "Object " << mId << " has invalid lighting layer ID " << mLightingLayerId;
            }
        }

        // TODO: finally write a deserializer for vectors of things!
        mLinks.resize(linkCount);
        for(size_t i = 0; i < linkCount; ++i)
        {
            dr >> mLinks[i];
        }

        dr >> xRot
           >> yRot
           >> zRot;

        mIsScaled = mFlags & OD_OBJECT_FLAG_SCALED;
        if(mIsScaled)
        {
            dr >> mInitialScale;

        }else
        {
            mInitialScale = glm::vec3(1, 1, 1);
        }

        odRfl::ObjectBuilderProbe builder; // it is important that we probe the fields before reading the record for this one
        mClass = mLevel.getAssetByRef<odDb::Class>(mClassRef);
        mRflClassInstance = mClass->makeInstance();
        if(mRflClassInstance != nullptr)
        {
            mRflClassInstance->probeFields(builder);

        }else
        {
            Logger::debug() << "Could not instantiate class of level object";
        }
        builder.readFieldRecord(dr); // this will read the field stuff and overwrite the fields in the class instance

        mInitialPosition *= OD_WORLD_SCALE; // correct editor scaling

        glm::vec3 eulerAngles(glm::radians((float)xRot), glm::radians((float)yRot-90), glm::radians((float)zRot)); // -90 deg. determined to be correct through experiment
        mInitialRotation = glm::quat(eulerAngles);

        mPosition = mInitialPosition;
        mRotation = mInitialRotation;
        mScale = mInitialScale;

        mIsVisible = mFlags & OD_OBJECT_FLAG_VISIBLE;

        if(mRflClassInstance != nullptr)
        {
            mRflClassInstance->onLoaded(*this); // do this last! we want to pass a fully set up level object here
        }
    }

    void LevelObject::spawned()
    {
        // build vector of linked object pointers from the stored indices if we haven't done that yet
        if(mLinkedObjects.size() != mLinks.size())
        {
            mLinkedObjects.clear();
            mLinkedObjects.reserve(mLinks.size());

            for(auto it = mLinks.begin(); it != mLinks.end(); ++it)
            {
                LevelObject *obj = mLevel.getLevelObjectByIndex(*it);
                if(obj == nullptr)
                {
                    Logger::warn() << "Object 0x" << std::hex << getObjectId() <<
                            " linked to nonexistent object 0x" << *it << std::dec;
                    continue;
                }

                mLinkedObjects.push_back(obj);
            }
        }

        // create render node if applicable
        odRender::Renderer *renderer = mLevel.getEngine().getRenderer();
        if(renderer != nullptr && mClass->hasModel())
        {
            mRenderNode = renderer->createObjectNode(*this);
            if(mRenderNode != nullptr)
            {
                od::RefPtr<odRender::ModelNode> model = mClass->getModel()->getOrCreateRenderNode(renderer);
                mRenderNode->setModel(model);

                mRenderNode->setPosition(mPosition);
                mRenderNode->setOrientation(mRotation);
                mRenderNode->setScale(mScale);

                if(mLightingLayer != nullptr)
                {
                    mRenderNode->setGlobalLight(mLightingLayer->getLightColor(), mLightingLayer->getAmbientColor(), mLightingLayer->getLightDirection());
                }
            }
        }

        // create physics handle if applicable
        odPhysics::PhysicsSystem &ps = mLevel.getEngine().getPhysicsSystem();
        bool shouldAutoCreatePhysics = true;
        if(mRflClassInstance != nullptr)
        {
            shouldAutoCreatePhysics = mRflClassInstance->onCreatePhysicsHandles(*this, ps);
        }

        if(shouldAutoCreatePhysics && mClass->hasModel())
        {
            // if the model does define bounds, create a regular collision handle. otherwise,
            //  create one without contact response so lighting still works
            bool hasPhysics = mClass->getModel()->getModelBounds().getShapeCount() != 0;
            mPhysicsHandle = ps.createObjectHandle(*this, !hasPhysics);
        }

        _setRenderNodeVisible(mIsVisible);

        _updateLayerBelowObject();

        if(mRflClassInstance != nullptr)
        {
            mRflClassInstance->onSpawned(*this);
        }

        mState = LevelObjectState::Spawned;
        Logger::debug() << "Object " << getObjectId() << " spawned";
    }

    void LevelObject::despawned()
    {
        if(mRflClassInstance != nullptr)
        {
            mRflClassInstance->onDespawned(*this);
        }

        Logger::debug() << "Object " << getObjectId() << " despawned";

        mRenderNode = nullptr;

        // TODO: how to ensure no unnecessary updates/cullings happen on our subgraph if present?

        // detach this from any object it may be attached to, and detach all objects attached to this
        detach();
        _detachAllAttachedObjects();

        mState = LevelObjectState::Loaded;
    }

    void LevelObject::destroyed()
    {
        Logger::verbose() << "Object " << getObjectId() << " destroyed";

        mState = LevelObjectState::Destroyed;
    }

    void LevelObject::update(float relTime)
    {
        if(mState != LevelObjectState::Spawned)
        {
            return;
        }

        if(mLayerBelowObjectDirty)
        {
            _updateLayerBelowObject();
        }

        if(mRflUpdateHookEnabled && mRflClassInstance != nullptr)
        {
            mRflClassInstance->onUpdate(*this, relTime);
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
            mRflClassInstance->onMessageReceived(*this, sender, message);
        }
    }

    void LevelObject::setPosition(const glm::vec3 &v)
    {
        mPosition = v;

        _onTransformChanged(this);
    }

    void LevelObject::setRotation(const glm::quat &q)
    {
        mRotation = q;

        _onTransformChanged(this);
    }

    void LevelObject::setScale(const glm::vec3 &scale)
    {
        mScale = scale;

        mIsScaled = (mScale != glm::vec3(1,1,1));

        _onTransformChanged(this);
    }

    void LevelObject::setVisible(bool v)
    {
        Logger::verbose() << "Object " << getObjectId() << " made " << (v ? "visible" : "invisible");

        mIsVisible = v;

        _setRenderNodeVisible(mIsVisible);
    }

    void LevelObject::setObjectType(LevelObjectType type)
    {
        mObjectType = type;

        switch(type)
        {
        case LevelObjectType::Normal:
            mIsVisible = true;
            break;

        case LevelObjectType::Light:
        case LevelObjectType::Detector:
        default:
            mIsVisible = false;
            break;
        }

        _setRenderNodeVisible(mIsVisible);
    }

    odAnim::Skeleton *LevelObject::getOrCreateSkeleton()
    {
        if(mSkeleton == nullptr && mClass->hasModel() && mClass->getModel()->hasSkeleton())
        {
            odDb::SkeletonBuilder *sb = mClass->getModel()->getSkeletonBuilder();
            mSkeleton = std::make_unique<odAnim::Skeleton>(sb->getJointCount());
            sb->build(*mSkeleton);
        }

        return mSkeleton.get();
    }

    void LevelObject::attachTo(LevelObject *target, bool ignoreTranslation, bool ignoreRotation, bool ignoreScale)
    {
        if(target == nullptr || mAttachmentTarget != nullptr)
        {
            detach();

            if(target == nullptr)
            {
                return;
            }
        }

        if(!ignoreRotation)
        {
            throw UnsupportedException("Attachment with rotation is unimplemented");
        }

        mAttachmentTarget = target;
        target->mAttachedObjects.push_back(this);

        mIgnoreAttachmentTranslation = ignoreTranslation;
        mIgnoreAttachmentRotation = ignoreRotation;
        mIgnoreAttachmentScale = ignoreScale;

        mAttachmentTranslationOffset = getPosition() - target->getPosition();
        mAttachmentRotationOffset = glm::inverse(getRotation()) * target->getRotation();
        mAttachmentScaleRatio = getScale() / target->getScale();
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

        auto it = std::find(mAttachmentTarget->mAttachedObjects.begin(), mAttachmentTarget->mAttachedObjects.end(), this);
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
        mRflUpdateHookEnabled = enableHook;
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

    void LevelObject::removeAffectingLight(od::Light *light)
    {
        if(light != nullptr && mRenderNode != nullptr)
        {
            mRenderNode->removeLight(light);
        }
    }

    void LevelObject::addAffectingLight(od::Light *light)
    {
        if(light != nullptr && mRenderNode != nullptr)
        {
            mRenderNode->addLight(light);
        }
    }

    void LevelObject::clearLightList()
    {
        if(mRenderNode != nullptr)
        {
            mRenderNode->clearLightList();
        }
    }

    void LevelObject::_onTransformChanged(LevelObject *transformChangeSource)
    {
        mLayerBelowObjectDirty = true;

        if(mRflClassInstance != nullptr)
        {
            mRflClassInstance->onMoved(*this);
        }

        for(auto it = mAttachedObjects.begin(); it != mAttachedObjects.end(); ++it)
        {
            (*it)->_attachmentTargetsTransformUpdated(this);
        }

        if(mRenderNode != nullptr)
        {
            mRenderNode->setPosition(mPosition);
            mRenderNode->setOrientation(mRotation);
            mRenderNode->setScale(mScale);
        }
    }

    void LevelObject::_updateLayerBelowObject()
    {
        mLayerBelowObject = mLevel.getFirstLayerBelowPoint(getPosition());

        if(mRenderNode != nullptr && mLayerBelowObject != nullptr && mLightingLayer == nullptr)
        {
            mRenderNode->setGlobalLight(mLayerBelowObject->getLightColor(), mLayerBelowObject->getAmbientColor(), mLayerBelowObject->getLightDirection());
        }

        mLayerBelowObjectDirty = false;
    }

    void LevelObject::_attachmentTargetsTransformUpdated(LevelObject *transformChangeSource)
    {
        if(transformChangeSource == this)
        {
            Logger::warn() << "Circular attachment detected! Will try to fix this by detaching one object in the loop";
            detach();
            return;
        }

        if(mAttachmentTarget == nullptr)
        {
            // uhmm.... wat?
            Logger::error() << "Attachment inconsistency: Level object that was not attached to anything was asked to update to attachment target position";
            return;
        }

        // note: don't call this->setPosition etc. here. it would call _onTransformChanged using this as argument, but we want
        //  to pass along transformChangeSource so we can detect circular attachments

        if(!mIgnoreAttachmentTranslation)
        {
            mPosition = mAttachmentTarget->getPosition() + mAttachmentTranslationOffset;
        }

        if(!mIgnoreAttachmentRotation)
        {
            // TODO: implement
        }

        if(!mIgnoreAttachmentScale)
        {
            mScale = mAttachmentTarget->getScale() * mAttachmentScaleRatio;
        }

        _onTransformChanged(transformChangeSource);
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

    void LevelObject::_setRenderNodeVisible(bool visible)
    {
        if(mRenderNode != nullptr)
        {
            mRenderNode->setVisible(visible);
        }
    }

}




