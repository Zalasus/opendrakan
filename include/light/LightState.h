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

    /**
     * @brief A StateAttribute handling a list of lights used internally by LightStateCallback.
     *
     * @note This should not be used directly. Use a LightStateCallback instead.
     */
    class LightStateAttribute : public osg::StateAttribute
    {
    public:

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


    /**
     * @brief A cull callback handling lighting.
     *
     * This callback handles applying the closest lights to a node when necessary (since in
     * forward rendering, the maximum number of lights in global state is limited).
     *
     * A fixed light can be defined that is always applied. This will most likely always be the layer light
     * associated with the object or layer.
     */
    class LightStateCallback : public osg::NodeCallback
    {
    public:

        /**
         * @brief Constructor for a LightStateCallback
         *
         * Creates a LightStateAttribute and attaches it to \c node.
         * The passed node is not necessarily the one the callback is attached to. It should be
         * a unique node that will have the same modelview matrix as the geometry to be lighted.
         *
         * If the passed node is shared among multiple LightStateCallback, the resulting light
         * behaviour is undefined.
         *
         * @param  lm    A LightManager to fetch lights from when updating light state
         * @param  node  The node on which the light state should be applied
         */
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
