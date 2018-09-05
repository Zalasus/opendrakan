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

    class LightStateAttribute : public osg::StateAttribute
    {
    public:

        LightStateAttribute();
        LightStateAttribute(const LightStateAttribute &l, const osg::CopyOp &copyOp = osg::CopyOp::SHALLOW_COPY);

        META_StateAttribute(od, LightStateAttribute, osg::StateAttribute::LIGHT);

        virtual void apply(osg::State &state) const override;
        virtual int compare(const StateAttribute& sa) const override;

        /**
         * @brief Adds a light to this state's list of potentially affecting lights.
         *
         * Only the closest of those lights will get applied until the maximum number of lights is reached.
         */
        void addLight(Light *l);


    private:

        std::vector<Light*> mPotentialLights;

    };


    class LightStateCallback : public osg::NodeCallback
    {
    public:

        LightStateCallback(LevelObject &obj);

        inline void lightingDirty() { mLightingDirty = true; }

        virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) override;


    private:

        void _updateLightState();

        LevelObject &mLevelObject;
        bool mLightingDirty;
        LightStateAttribute *mLightStateAttribute;

    };

}


#endif /* INCLUDE_LIGHT_LIGHTSTATE_H_ */
