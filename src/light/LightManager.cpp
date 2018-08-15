/*
 * LightManager.cpp
 *
 *  Created on: Aug 14, 2018
 *      Author: zal
 */

#include "light/LightManager.h"

namespace od
{


    LightManager::LightManager(Engine &engine, osg::Group *sceneRoot)
    : mEngine(engine)
    , mSceneRoot(sceneRoot)
    {
    }

    Light *LightManager::addLight(LevelObject *obj)
    {
        osg::ref_ptr<Light> newLight = new Light(obj);

        mLights.push_back(newLight);

        return newLight;
    }



}
