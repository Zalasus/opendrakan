/*
 * Handle.h
 *
 *  Created on: May 15, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_RENDER_HANDLE_H_
#define INCLUDE_ODOSG_RENDER_HANDLE_H_

#include <odCore/render/Handle.h>

namespace odOsg
{

    class Handle : public odRender::Handle
    {
    public:

        virtual std::mutex &getMutex() override;

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
        virtual void setGlobalLight(const glm::vec3 &direction, const glm::vec3 &diffuse, const glm::vec3 &ambient) override;

    };

}


#endif /* INCLUDE_ODOSG_RENDER_HANDLE_H_ */
