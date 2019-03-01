/*
 * Constants.h
 *
 *  Created on: Mar 1, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_CONSTANTS_H_
#define INCLUDE_ODOSG_CONSTANTS_H_

#include <stdint.h>

namespace odOsg
{

    class Constants
    {
    public:

        /**
         * @brief Maximum number of dynamic lights to allow to be rendered on layers.
         *
         * Static lights are not included in this count as they are baked into the vertex colors by
         * the engine core, and thus have no limit that the renderer would have to handle.
         */
        static const uint32_t MAX_DYNAMIC_LIGHTS_LAYER = 16;

        /**
         * @brief Maximum number of lights to allow to be rendered on objects.
         *
         * This includes both static and dynamic lights as we do not bake static lighting into the
         * vertices of objects, because this is only possible on models that are only used once and are
         * itself static. Thus, the performance gain from doing so would be neglectable.
         */
        static const uint32_t MAX_LIGHTS_OBJECT = 16;

        /**
         * @brief Maximum number of bones to account for in the rigging shader.
         */
        static const uint32_t MAX_BONES = 64;

        /**
         * @brief Default fullscreen gamma. This probably should be a config default instead.
         */
        static const float DEFAULT_GAMMA = 1.5f;

        /**
         * @brief Path relative to engine executable where shader sources are to be found.
         */
        static const char *SHADER_SOURCE_PATH = "resources/shader_src";

    };

}


#endif /* INCLUDE_ODOSG_CONSTANTS_H_ */
