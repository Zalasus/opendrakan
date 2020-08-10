/*
 * Handle.h
 *
 *  Created on: May 15, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_RENDER_HANDLE_H_
#define INCLUDE_ODOSG_RENDER_HANDLE_H_

#include <memory>

#include <osg/PositionAttitudeTransform>
#include <osg/Depth>
#include <osg/Uniform>
#include <osg/Viewport>

#include <odCore/render/Handle.h>
#include <odCore/render/Renderer.h>

#include <odOsg/render/LightState.h>

namespace odOsg
{
    class Renderer;
    class Model;
    class Rig;

    class Handle : public odRender::Handle
    {
    public:

        Handle(Renderer &renderer);
        virtual ~Handle();

        inline osg::Group *getOsgNode() { return mTransform; }
        inline osg::Group *getParentOsgGroup() { return mParentGroup; }
        inline void setParentOsgGroup(osg::Group *p) { mParentGroup = p; }

        virtual std::mutex &getMutex() override;

        virtual glm::vec3 getPosition() override;
        virtual glm::quat getOrientation() override;
        virtual glm::vec3 getScale() override;
        virtual void setPosition(const glm::vec3 &pos) override;
        virtual void setOrientation(const glm::quat &orientation) override;
        virtual void setScale(const glm::vec3 &scale) override;

        virtual odRender::Model *getModel() override;
        virtual void setModel(std::shared_ptr<odRender::Model> model) override;

        virtual void setVisible(bool visible) override;
        virtual void setModelPartVisible(size_t partIndex, bool visible) override;

        virtual void setRenderBin(odRender::RenderBin rm) override;

        virtual void addFrameListener(odRender::FrameListener *listener) override;
        virtual void removeFrameListener(odRender::FrameListener *listener) override;

        virtual void setEnableColorModifier(bool b) override;
        virtual void setColorModifier(const glm::vec4 &cm) override;

        virtual odRender::Rig *getRig() override;

        virtual void addLight(std::shared_ptr<od::Light> light) override;
        virtual void removeLight(std::shared_ptr<od::Light> light) override;
        virtual void clearLightList() override;

        /**
         * @brief Sets the global directional light that is always applied to the object if it is shaded.
         *
         * @param diffuse    Diffuse component of the light
         * @param ambient    Ambient component of the light
         * @param direction  Direction vector pointing towards the light source
         */
        virtual void setGlobalLight(const glm::vec3 &direction, const glm::vec3 &diffuse, const glm::vec3 &ambient) override;

        void update(double simTime, double relTime, uint32_t frameNumber);


    private:

        osg::ref_ptr<osg::Group> mParentGroup;

        std::mutex mMutex;
        std::shared_ptr<Model> mModel;
        odRender::FrameListener *mFrameListener;
        osg::ref_ptr<osg::Callback> mUpdateCallback;
        osg::ref_ptr<osg::PositionAttitudeTransform> mTransform;
        osg::ref_ptr<osg::Depth> mDepth;
        osg::ref_ptr<osg::Uniform> mColorModifierUniform;
        osg::ref_ptr<LightStateAttribute> mLightStateAttribute;
        osg::ref_ptr<osg::Viewport> mViewport;

        std::unique_ptr<Rig> mRig;
    };

}


#endif /* INCLUDE_ODOSG_RENDER_HANDLE_H_ */
