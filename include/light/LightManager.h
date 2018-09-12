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

        void addLight(Light *light);
        void removeLight(Light *light);

        /**
         * @brief Fills the passed vector with all registered lights whose radius includes the given point in world space.
         */
        void getLightsAffectingPoint(const osg::Vec3 &point, std::vector<Light*> &lights);

        void getLightsIntersectingSphere(const osg::BoundingSphere &sphere, std::vector<Light*> &lights);

        void applyLayerLight(const osg::Matrix &viewMatrix, const osg::Vec3 &color, const osg::Vec3 &ambient, const osg::Vec3 &direction);
        void applyToLightUniform(const osg::Matrix &viewMatrix, Light *light, size_t index);
        void applyNullLight(size_t index);

        virtual void objectDeleted(void *object) override;


    private:

        Engine &mEngine;
        std::vector<Light*> mLights;

        osg::ref_ptr<osg::Uniform> mLayerLightDiffuse;
        osg::ref_ptr<osg::Uniform> mLayerLightAmbient;
        osg::ref_ptr<osg::Uniform> mLayerLightDirection;

        osg::ref_ptr<osg::Uniform> mLightDiffuseColors;
        osg::ref_ptr<osg::Uniform> mLightIntensities;
        osg::ref_ptr<osg::Uniform> mLightRadii;
        osg::ref_ptr<osg::Uniform> mLightPositions;
    };

}


#endif /* INCLUDE_LIGHT_LIGHTMANAGER_H_ */
