/*
 * Handle.h
 *
 *  Created on: May 15, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_RENDER_HANDLE_H_
#define INCLUDE_ODOSG_RENDER_HANDLE_H_

#include <osg/PositionAttitudeTransform>
#include <osg/Depth>
#include <osg/Uniform>

#include <odCore/render/Handle.h>

#include <odOsg/render/LightState.h>

namespace odOsg
{
    class Renderer;
    class Model;

    class Handle : public odRender::Handle
    {
    public:

        Handle(Renderer *renderer, osg::Group *parent);
        virtual ~Handle();

        virtual std::mutex &getMutex() override;

        virtual glm::vec3 getPosition() override;
        virtual glm::quat getOrientation() override;
        virtual glm::vec3 getScale() override;
        virtual void setPosition(const glm::vec3 &pos) override;
        virtual void setOrientation(const glm::quat &orientation) override;
        virtual void setScale(const glm::vec3 &scale) override;

        virtual odRender::Model *getModel() override;
        virtual void setModel(odRender::Model *model) override;

        virtual void setVisible(bool visible) override;
        virtual void setModelPartVisible(size_t partIndex, bool visible) override;

        virtual void setRenderMode(odRender::RenderMode rm) override;

        virtual void addFrameListener(odRender::FrameListener *listener) override;
        virtual void removeFrameListener(odRender::FrameListener *listener) override;

        virtual void setEnableColorModifier(bool b) override;
        virtual void setColorModifier(const glm::vec4 &cm) override;

        virtual odRender::Rig *getRig() override;

        virtual void addLight(od::Light *light) override;
        virtual void removeLight(od::Light *light) override;
        virtual void clearLightList() override;

        /**
         * @brief Sets the global directional light that is always applied to the object if it is shaded.
         *
         * @param diffuse    Diffuse component of the light
         * @param ambient    Ambient component of the light
         * @param direction  Direction vector pointing towards the light source
         */
        virtual void setGlobalLight(const glm::vec3 &direction, const glm::vec3 &diffuse, const glm::vec3 &ambient) override;


    private:

        osg::ref_ptr<osg::Group> mParentGroup;

        std::mutex mMutex;
        Model *mModel;
        osg::ref_ptr<osg::PositionAttitudeTransform> mTransform;
        osg::ref_ptr<osg::Depth> mDepth;
        osg::ref_ptr<osg::Uniform> mColorModifierUniform;
        osg::ref_ptr<LightStateAttribute> mLightStateAttribute;
    };

}


#endif /* INCLUDE_ODOSG_RENDER_HANDLE_H_ */
