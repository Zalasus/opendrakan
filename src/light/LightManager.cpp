/*
 * LightManager.cpp
 *
 *  Created on: Aug 14, 2018
 *      Author: zal
 */

#include "light/LightManager.h"

#include <osg/Light>

namespace od
{


    LightManager::LightManager(Engine &engine, osg::Group *sceneRoot)
    : mEngine(engine)
    , mSceneRoot(sceneRoot)
    {
        // attach a default light state to the root node.
        //  since we are using shaders, we'd have to pass a separate uniform to directly enable/disable
        //  individual lights. we might do this via a custom StateAttribute encapsulating the glLight calls, but for
        //  now we just sat all lights to black.
        osg::StateSet *ss = mSceneRoot->getOrCreateStateSet();
        for(size_t i = 0; i < 8; ++i) // FIXME: get max number of lights
        {
            osg::ref_ptr<osg::Light> light = new osg::Light(i);
            light->setDiffuse(osg::Vec4(0.0, 0.0, 0.0, 0.0));
            light->setAmbient(osg::Vec4(0.0, 0.0, 0.0, 0.0));
            light->setPosition(osg::Vec4(1.0, 0.0, 0.0, 0.0));
            ss->setAttribute(light, osg::StateAttribute::ON);
        }
    }

    Light *LightManager::addLight(LevelObject *obj)
    {
        osg::ref_ptr<Light> newLight = new Light(obj);

        mLights.push_back(newLight);

        return newLight;
    }



}
