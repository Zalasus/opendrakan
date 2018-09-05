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

        LightStateAttribute();
        LightStateAttribute(const LightStateAttribute &l, const osg::CopyOp &copyOp = osg::CopyOp::SHALLOW_COPY);

        META_StateAttribute(od, LightStateAttribute, osg::StateAttribute::LIGHT);

        virtual void apply(osg::State &state) const override;
        virtual int compare(const StateAttribute& sa) const override;

        /**
         * @brief Adds a light to this state's list of affecting lights.
         *
         * If more lights than the maximum possible number of lights are added, the additional calls are ignored.
         */
        void addLight(LightHandle *lightHandle);


    private:

        std::vector<LightHandle*> mLights;

    };


    class LightStateCallback : public osg::NodeCallback
    {
    public:

        LightStateCallback(LightManager &lm, LevelObject &obj);

        inline void lightingDirty() { mLightingDirty = true; }

        virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) override;


    private:

        void _updateLightState();

        LightManager &mLightManager;
        LevelObject &mLevelObject;
        bool mLightingDirty;
        LightStateAttribute *mLightStateAttribute;

    };

}


#endif /* INCLUDE_LIGHT_LIGHTSTATE_H_ */
