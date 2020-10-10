/*
 * Class.cpp
 *
 *  Created on: Sep 8, 2019
 *      Author: zal
 */

#include <odCore/rfl/Class.h>

#include <odCore/Exception.h>

#include <odCore/physics/Handles.h>

namespace odRfl
{

    FieldBundle::~FieldBundle()
    {
    }


    ClassBase::~ClassBase()
    {
    }

    void ClassBase::onLoaded()
    {
    }


    SpawnableClass::~SpawnableClass()
    {
    }

    void SpawnableClass::onSpawned()
    {
    }

    void SpawnableClass::onDespawned()
    {
    }

    void SpawnableClass::onStart()
    {
    }

    void SpawnableClass::onStop()
    {
    }

    void SpawnableClass::onUpdate(float relTime)
    {
    }

    void SpawnableClass::onPostUpdate(float relTime)
    {
    }

    void SpawnableClass::onMessageReceived(od::LevelObject &sender, od::Message message)
    {
    }

    void SpawnableClass::onVisibilityChanged(bool newVisibility)
    {
    }

    void SpawnableClass::onLayerChanged(od::Layer *from, od::Layer *to)
    {
    }

    void SpawnableClass::onTranslated(const glm::vec3 &from, const glm::vec3 &to)
    {
    }

    void SpawnableClass::onRotated(const glm::quat &from, const glm::quat &to)
    {
    }

    void SpawnableClass::onScaled(const glm::vec3 &from, const glm::vec3 &to)
    {
    }

    void SpawnableClass::onTransformChanged()
    {
    }


    ClassFactory::~ClassFactory()
    {
    }


}
