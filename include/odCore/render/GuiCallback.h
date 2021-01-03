
#ifndef INCLUDE_ODCORE_RENDER_GUICALLBACK_H_
#define INCLUDE_ODCORE_RENDER_GUICALLBACK_H_

namespace odRender
{

    class GuiCallback
    {
    public:

        virtual void onUpdate(float relTime) = 0;
        virtual void onFramebufferResize(glm::vec2 dimensionsPx) = 0;

    };

}

#endif
