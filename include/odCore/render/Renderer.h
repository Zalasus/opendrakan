/*
 * Renderer.h
 *
 *  Created on: Nov 10, 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_RENDERER_H_
#define INCLUDE_ODCORE_RENDER_RENDERER_H_

namespace od
{
    class LevelObject;
    class Layer;
}

namespace odDb
{
    class Model;
    class Texture;
}

namespace odRender
{

    class ObjectNode;
    class ModelNode;
    class Texture;

    /**
     * Interface for a renderer implementation.
     */
    class Renderer
    {
    public:

        virtual ~Renderer() = default;

        inline void toggleLighting() { setEnableLighting(!isLightingEnabled()); }

        virtual void onStart() = 0;
        virtual void onEnd() = 0;

        virtual void setEnableLighting(bool b) = 0;
        virtual bool isLightingEnabled() const = 0;

        virtual ObjectNode *createObjectNode(od::LevelObject &obj) = 0;
        virtual ModelNode *getNodeForModel(odDb::Model *model) = 0;

    };

}


#endif /* INCLUDE_ODCORE_RENDER_RENDERER_H_ */
