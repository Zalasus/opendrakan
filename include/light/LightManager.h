/*
 * LightManager.h
 *
 *  Created on: Aug 14, 2018
 *      Author: zal
 */

#ifndef INCLUDE_LIGHT_LIGHTMANAGER_H_
#define INCLUDE_LIGHT_LIGHTMANAGER_H_

#include <osg/Group>
#include <vector>

#include "light/Light.h"

namespace od
{
    class Engine;
    class LevelObject;

    class LightManager
    {
    public:

        LightManager(Engine &engine, osg::Group *sceneRoot);

        Light *addLight(LevelObject *obj);


    private:

        Engine &mEngine;
        osg::ref_ptr<osg::Group> mSceneRoot;
        std::vector<osg::ref_ptr<Light>> mLights;

    };

}


#endif /* INCLUDE_LIGHT_LIGHTMANAGER_H_ */
