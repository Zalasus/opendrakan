/**
 * @file Constants.h
 * @date Jan 27, 2020
 * @author zal
 */

#ifndef INCLUDE_ODGL_CONSTANTS_H_
#define INCLUDE_ODGL_CONSTANTS_H_

#include <odGl/GL.h>

namespace odGl
{

    class Constants
    {
    public:

        static constexpr GLuint ATTRIBUTE_POSITION_LOCATION = 0;
        static constexpr GLuint ATTRIBUTE_NORMAL_LOCATION = 1;
        static constexpr GLuint ATTRIBUTE_COLOR_LOCATION = 2;
        static constexpr GLuint ATTRIBUTE_TEXTURECOORD_LOCATION = 3;
    };

}


#endif /* INCLUDE_ODGL_CONSTANTS_H_ */
