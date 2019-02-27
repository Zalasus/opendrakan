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

        LightStateAttribute(Renderer *renderer);
        LightStateAttribute(const LightStateAttribute &l, const osg::CopyOp &copyOp = osg::CopyOp::SHALLOW_COPY);

        // implement pure virtual stuff from osg::StateAttribute
        virtual const char* libraryName() const { return "odOsg"; }
        virtual const char* className() const { return "LightStateAttribute"; }
        virtual Type getType() const { return osg::StateAttribute::LIGHT; }
        virtual osg::Object *cloneType() const;
        virtual osg::Object *clone(const osg::CopyOp& copyop) const;
        virtual bool isSameKindAs(const osg::Object* obj) const;

        inline void setLayerLight(const osg::Vec3 &color, const osg::Vec3 &ambient, const osg::Vec3 &direction)
        {
            mLayerLightDiffuse = color;
            mLayerLightAmbient = ambient;
            mLayerLightDirection = direction;
        }

        virtual bool getModeUsage(ModeUsage &usage) const override;
        virtual void apply(osg::State &state) const override;
        virtual int compare(const StateAttribute& sa) const override;

        void clearLightList();

        /**
         * @brief Adds a light to this state's list of affecting lights.
         *
         * If more lights than the maximum possible number of lights are added, the additional calls are ignored.
         */
        void addLight(od::Light *light);

        void removeLight(od::Light *light);


    private:

        Renderer *mRenderer;
        std::vector<od::RefPtr<od::Light>> mLights;
        osg::Vec3 mLayerLightDiffuse;
        osg::Vec3 mLayerLightAmbient;
        osg::Vec3 mLayerLightDirection;

    };

}

#endif /* INCLUDE_ODOSG_LIGHTSTATE_H_ */
