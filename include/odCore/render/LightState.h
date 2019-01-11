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

#include <odCore/render/Light.h>

namespace odRender
{

    class RenderManager;

    /**
     * @brief A StateAttribute handling a list of lights used internally by LightStateCallback.
     *
     * @note This should not be used directly. Use a LightStateCallback instead.
     */
    class LightStateAttribute : public osg::StateAttribute
    {
    public:

        LightStateAttribute(RenderManager &rm);
        LightStateAttribute(const LightStateAttribute &l, const osg::CopyOp &copyOp = osg::CopyOp::SHALLOW_COPY);

        // implement pure virtual stuff from osg::StateAttribute
        virtual const char* libraryName() const { return "od"; }
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
        void addLight(Light *light);


    private:

        RenderManager &mRenderManager;
        std::vector<osg::ref_ptr<Light>> mLights;
        osg::Vec3 mLayerLightDiffuse;
        osg::Vec3 mLayerLightAmbient;
        osg::Vec3 mLayerLightDirection;

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
         * FIXME: The ignoreCulledState is a dirty hack to get layers to update. For some reason, they
         * are always reported as being culled right now.
         *
         * @param rm                 A RenderManager to fetch lights from when updating light state
         * @param node               The node on which the light state should be applied
         * @param ignoreCulledState  Whether the callback should update lighting even if the node is culled
         */
        LightStateCallback(RenderManager &rm, osg::Node *node, bool ignoreCulledState = false);

        inline void lightingDirty() { mLightingDirty = true; }
        inline void setLayerLight(const osg::Vec3 &color, const osg::Vec3 &ambient, const osg::Vec3 &direction)
        {
            mLightStateAttribute->setLayerLight(color, ambient, direction);
        }

        virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) override;


    private:

        void _updateLightState(osg::Node *node);

        RenderManager &mRenderManager;
        bool mIgnoreCulledState;
        bool mLightingDirty;
        LightStateAttribute *mLightStateAttribute;
        std::vector<Light*> mTmpAffectingLightsList;
    };

}


#endif /* INCLUDE_LIGHT_LIGHTSTATE_H_ */