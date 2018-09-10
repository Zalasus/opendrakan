/*
 * LightState.h
 *
 *  Created on: Sep 4, 2018
 *      Author: zal
 */

#ifndef INCLUDE_LIGHT_LIGHTSTATE_H_
#define INCLUDE_LIGHT_LIGHTSTATE_H_

#include <osg/StateAttribute>
#include <osg/NodeCallback>

#include "light/Light.h"

namespace od
{

    class LightManager;


    class LightStateAttribute : public osg::StateAttribute
    {
    public:

        friend class LightStateCallback;

        LightStateAttribute();
        LightStateAttribute(const LightStateAttribute &l, const osg::CopyOp &copyOp = osg::CopyOp::SHALLOW_COPY);

        META_StateAttribute(od, LightStateAttribute, osg::StateAttribute::LIGHT);

        virtual bool getModeUsage(ModeUsage &usage) const override;
        virtual void apply(osg::State &state) const override;
        virtual int compare(const StateAttribute& sa) const override;

        void clearLightList();

        /**
         * @brief Adds a light to this state's list of affecting lights.
         *
         * If more lights than the maximum possible number of lights are added, the additional calls are ignored.
         */
        void addLight(osg::Light *light);


    private:

        std::vector<osg::ref_ptr<osg::Light>> mLights;
        osg::ref_ptr<osg::Light> mNullLight;

    };


    class LightStateCallback : public osg::NodeCallback
    {
    public:

        LightStateCallback(LightManager &lm, osg::Node *node);

        inline void lightingDirty() { mLightingDirty = true; }
        inline void setFixedLight(osg::Light *light) { mFixedLight = light; }

        virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) override;


    private:

        void _updateLightState(osg::Node *node);

        LightManager &mLightManager;
        bool mLightingDirty;
        LightStateAttribute *mLightStateAttribute;
        std::vector<LightHandle*> mAffectingLightsCache;
        osg::ref_ptr<osg::Light> mFixedLight;
    };

}


#endif /* INCLUDE_LIGHT_LIGHTSTATE_H_ */
