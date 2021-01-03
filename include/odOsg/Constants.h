/*
 * Constants.h
 *
 *  Created on: Mar 1, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_CONSTANTS_H_
#define INCLUDE_ODOSG_CONSTANTS_H_

#include <odCore/CTypes.h>

namespace odOsg
{

    class Constants
    {
    public:

        /**
         * @brief Maximum number of lights to allow to be rendered on objects and layers.
         *
         * For layers, this does not include static lights, as they are baked into the vertex
         * colors by the engine core, and thus have no limit that the renderer would have to handle.
         */
        static constexpr uint32_t MAX_LIGHTS = 8;

        /**
         * @brief Maximum number of bones to account for in the rigging shader.
         */
        static constexpr uint32_t MAX_BONES = 64;

        /**
         * @brief  Location of the bone influence attribute for the rigging shader.
         */
        static constexpr uint32_t ATTRIB_INFLUENCE_LOCATION = 4;

        /**
         * @brief Location of the bone weight attribute for the rigging shader.
         */
        static constexpr uint32_t ATTRIB_WEIGHT_LOCATION = 5;

        /**
         * @brief Default fullscreen gamma. This probably should be a config default instead.
         */
        static constexpr float DEFAULT_GAMMA = 1.5f;

        /**
         * @brief Path relative to engine executable where shader sources are to be found.
         */
        static constexpr const char *SHADER_SOURCE_PATH = "resources/shader_src";


        /**
         * @brief Default sample rate to be used for audio output.
         */
        static constexpr uint32_t AUDIO_SAMPLE_RATE = 44100;

    };

}


#endif /* INCLUDE_ODOSG_CONSTANTS_H_ */
