/*
 * LightManager.cpp
 *
 *  Created on: Aug 14, 2018
 *      Author: zal
 */

#include "light/LightManager.h"

#include <osg/Light>
#include <osg/Material>

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
            light->setSpecular(osg::Vec4(0.0, 0.0, 0.0, 0.0));
            light->setPosition(osg::Vec4(1.0, 0.0, 0.0, 0.0));
            ss->setAttribute(light, osg::StateAttribute::ON);
        }

        osg::ref_ptr<osg::Material> defaultMaterial(new osg::Material);
        defaultMaterial->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0, 1.0, 1.0, 1.0));
        defaultMaterial->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0, 1.0, 1.0, 1.0));
        defaultMaterial->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0, 0.0, 0.0, 0.0));
        defaultMaterial->setShininess(osg::Material::FRONT_AND_BACK, 1.0); // do NOT set this to 0! somehow it breaks the GLSL pow function
        ss->setAttribute(defaultMaterial, osg::StateAttribute::ON);
    }

    Light *LightManager::addLight(LevelObject *obj)
    {
        osg::ref_ptr<Light> newLight = new Light(obj);

        mLights.push_back(newLight);

        return newLight;
    }



}
