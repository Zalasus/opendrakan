/*
 * Geometry.h
 *
 *  Created on: Nov 12, 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_GEOMETRY_H_
#define INCLUDE_ODCORE_RENDER_GEOMETRY_H_

#include <vector>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace odRender
{

    class Geometry
    {
    public:

        std::vector<glm::vec3> &getVertexArray();
        std::vector<glm::vec4> &getColorArray();
        std::vector<glm::vec3> &getNormalArray();

        std::vector<glm::vec4> &getBoneIndexArray();
        std::vector<glm::vec4> &getBoneWeightArray();

    };

}


#endif /* INCLUDE_ODCORE_RENDER_GEOMETRY_H_ */
