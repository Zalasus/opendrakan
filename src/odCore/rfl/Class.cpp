/*
 * Class.cpp
 *
 *  Created on: Sep 8, 2019
 *      Author: zal
 */

#include <odCore/rfl/Class.h>

#include <odCore/Exception.h>

namespace odRfl
{

    ClassBase::ClassBase()
    : mRfl(nullptr)
    {
    }

    void ClassBase::setRfl(Rfl &rfl)
    {
        if(mRfl != nullptr)
        {
            throw od::Exception("RFL already assigned");
        }

        mRfl = &rfl;
    }

    Rfl &ClassBase::getRfl()
    {
        if(mRfl == nullptr)
        {
            throw od::Exception("No RFL assigned");
        }

        return *mRfl;
    }

    void ClassBase::onLoaded()
    {
    }

    ClassBaseType ClassBase::getBaseType()
    {
        return ClassBaseType::DEFAULT;
    }

    LevelObjectClassBase *ClassBase::asLevelObjectBase()
    {
        return nullptr;
    }

    MaterialClassBase *ClassBase::asMaterialBase()
    {
        return nullptr;
    }


    LevelObjectClassBase::LevelObjectClassBase()
    : mLevelObject(nullptr)
    {
    }

    void LevelObjectClassBase::setLevelObject(od::LevelObject &obj)
    {
        if(mLevelObject != nullptr)
        {
            throw od::Exception("LevelObject already assigned");
        }

        mLevelObject = &obj;
    }

    od::LevelObject &LevelObjectClassBase::getLevelObject()
    {
        if(mLevelObject == nullptr)
        {
            throw od::Exception("No LevelObject assigned");
        }

        return *mLevelObject;
    }

    void LevelObjectClassBase::onSpawned()
    {
    }

    void LevelObjectClassBase::onDespawned()
    {
    }

    void LevelObjectClassBase::onUpdate(float relTime)
    {
    }

    void LevelObjectClassBase::onMessageReceived(od::LevelObject &sender, od::Message message)
    {
    }

    void LevelObjectClassBase::onDestroyed()
    {
    }

    void LevelObjectClassBase::onVisibilityChanged()
    {
    }

    void LevelObjectClassBase::onLayerChanged(od::Layer *from, od::Layer *to)
    {
    }

    void LevelObjectClassBase::onTranslated(const glm::vec3 &from, const glm::vec3 &to)
    {
    }

    void LevelObjectClassBase::onRotated(const glm::quat &from, const glm::quat &to)
    {
    }

    void LevelObjectClassBase::onScaled(const glm::vec3 &from, const glm::vec3 &to)
    {
    }

    void LevelObjectClassBase::onTransformChanged()
    {
    }

    ClassBaseType LevelObjectClassBase::getBaseType()
    {
        return ClassBaseType::LEVEL_OBJECT;
    }

    LevelObjectClassBase *LevelObjectClassBase::asLevelObjectBase()
    {
        return this;
    }


    ClassBaseType MaterialClassBase::getBaseType()
    {
        return ClassBaseType::MATERIAL;
    }

    MaterialClassBase *MaterialClassBase::asMaterialBase()
    {
        return this;
    }

}
