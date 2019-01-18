/*
 * LightState.h
 *
 *  Created on: 27 Nov 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_LIGHTSTATE_H_
#define INCLUDE_ODOSG_LIGHTSTATE_H_

#include <osg/Vec3>
#include <osg/StateAttribute>
#include <osg/NodeCallback>

#include <odCore/render/Light.h>

namespace odOsg
{

    class Renderer;

    /**
     * @brief A StateAttribute handling a list of lights used internally by LightStateCallback.
     *
     * @note This should not be used directly. Use a LightStateCallback instead.
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
        void addLight(odRender::Light *light);


    private:

        Renderer *mRenderer;
        std::vector<od::RefPtr<odRender::Light>> mLights;
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
         * @param renderer           An odOsg::Renderer to fetch lights from when updating light state
         * @param node               The node on which the light state should be applied
         * @param ignoreCulledState  Whether the callback should update lighting even if the node is culled
         */
        LightStateCallback(Renderer *renderer, osg::Node *node, bool ignoreCulledState = false);

        inline void lightingDirty() { mLightingDirty = true; }
        inline void setLightMask(uint32_t lightMask) { mLightMask = lightMask; }
        inline void setLayerLight(const osg::Vec3 &color, const osg::Vec3 &ambient, const osg::Vec3 &direction)
        {
            mLightStateAttribute->setLayerLight(color, ambient, direction);
        }

        virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) override;


    private:

        void _updateLightState(osg::Node *node);

        Renderer *mRenderer;
        bool mIgnoreCulledState;
        bool mLightingDirty;
        uint32_t mLightMask;
        LightStateAttribute *mLightStateAttribute;
        std::vector<odRender::Light*> mTmpAffectingLightsList;
    };

}

#endif /* INCLUDE_ODOSG_LIGHTSTATE_H_ */
