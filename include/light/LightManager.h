/*
 * LightManager.h
 *
 *  Created on: Aug 14, 2018
 *      Author: zal
 */

#ifndef INCLUDE_LIGHT_LIGHTMANAGER_H_
#define INCLUDE_LIGHT_LIGHTMANAGER_H_

#include <vector>
#include <osg/Group>
#include <osg/Observer>

#include "light/Light.h"

namespace od
{
    class Engine;
    class LevelObject;

    class LightManager : public osg::Observer
    {
    public:

        LightManager(Engine &engine, osg::Group *sceneRoot);

        void addLight(LightHandle *lightHandle);
        void removeLight(LightHandle *lightHandle);

        /**
         * @brief Fills the passed vector with all registered lights whose radius includes the given point in world space.
         */
        void getLightsAffectingPoint(const osg::Vec3 &point, std::vector<LightHandle*> &lights);

        void getLightsIntersectingSphere(const osg::BoundingSphere &sphere, std::vector<LightHandle*> &lights);

        virtual void objectDeleted(void *object) override;


    private:

        Engine &mEngine;
        osg::ref_ptr<osg::Group> mSceneRoot;
        std::vector<LightHandle*> mLightHandles;

    };

}


#endif /* INCLUDE_LIGHT_LIGHTMANAGER_H_ */
