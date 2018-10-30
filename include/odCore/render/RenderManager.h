/*
 * RenderManager.h
 *
 *  Created on: Oct 27, 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_RENDERMANAGER_H_
#define INCLUDE_ODCORE_RENDER_RENDERMANAGER_H_

#include <vector>
#include <osg/Group>
#include <osg/Observer>

#include <odCore/render/Light.h>
#include <odCore/render/ShaderFactory.h>

namespace od
{
    class Engine;
}

namespace odRender
{

    class RenderManager : public osg::Observer
    {
    public:

        RenderManager(od::Engine &engine, osg::Group *sceneRoot);

        inline ShaderFactory &getShaderFactory() { return mShaderFactory; }
        inline bool isLightingEnabled() const { return mLightingEnabled; }

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

        void setEnableLighting(bool b);
        void toggleLighting() { setEnableLighting(!mLightingEnabled); }

        // implement observer for lights
        virtual void objectDeleted(void *object) override;


    private:

        od::Engine &mEngine;
        ShaderFactory mShaderFactory;
        std::vector<Light*> mLights;
        bool mLightingEnabled;

        osg::ref_ptr<osg::Uniform> mLayerLightDiffuse;
        osg::ref_ptr<osg::Uniform> mLayerLightAmbient;
        osg::ref_ptr<osg::Uniform> mLayerLightDirection;

        osg::ref_ptr<osg::Uniform> mLightDiffuseColors;
        osg::ref_ptr<osg::Uniform> mLightIntensities;
        osg::ref_ptr<osg::Uniform> mLightRadii;
        osg::ref_ptr<osg::Uniform> mLightPositions;
    };

}


#endif /* INCLUDE_ODCORE_RENDER_RENDERMANAGER_H_ */
