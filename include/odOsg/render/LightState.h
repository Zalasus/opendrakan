/*
 * LightState.h
 *
 *  Created on: 27 Nov 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_LIGHTSTATE_H_
#define INCLUDE_ODOSG_LIGHTSTATE_H_

#include <vector>

#include <osg/Vec3>
#include <osg/StateAttribute>
#include <osg/NodeCallback>

#include <odCore/Light.h>

namespace odOsg
{

    class Renderer;

    /**
     * @brief A StateAttribute handling a list of lights used internally by LightStateCallback.
     */
    class LightStateAttribute : public osg::StateAttribute
    {
    public:

        LightStateAttribute(Renderer &renderer, size_t maxLightCount);
        LightStateAttribute(const LightStateAttribute &l, const osg::CopyOp &copyOp = osg::CopyOp::SHALLOW_COPY);

        // implement pure virtual stuff from osg::StateAttribute
        virtual const char* libraryName() const override { return "odOsg"; }
        virtual const char* className() const override { return "LightStateAttribute"; }
        virtual Type getType() const override { return osg::StateAttribute::LIGHT; }
        virtual osg::Object *cloneType() const override;
        virtual osg::Object *clone(const osg::CopyOp& copyop) const override;
        virtual bool isSameKindAs(const osg::Object* obj) const override;
        virtual bool getModeUsage(ModeUsage &usage) const override;
        virtual void apply(osg::State &state) const override;
        virtual int compare(const StateAttribute& sa) const override;

        inline void setLayerLight(const osg::Vec3 &color, const osg::Vec3 &ambient, const osg::Vec3 &direction)
        {
            mLayerLightDiffuse = color;
            mLayerLightAmbient = ambient;
            mLayerLightDirection = direction;
        }

        void clearLightList();

        /**
         * @brief Adds a light to this state's list of affecting lights.
         *
         * If more lights than the maximum possible number of lights are added, the additional calls are ignored.
         */
        void addLight(std::shared_ptr<od::Light> light);

        void removeLight(od::Light *light);


    private:

        Renderer &mRenderer;
        size_t mMaxLightCount;
        std::vector<std::weak_ptr<od::Light>> mLights;
        osg::Vec3 mLayerLightDiffuse;
        osg::Vec3 mLayerLightAmbient;
        osg::Vec3 mLayerLightDirection;

    };

}

#endif /* INCLUDE_ODOSG_LIGHTSTATE_H_ */
