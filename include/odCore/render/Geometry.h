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

        virtual ~Geometry() = default;

        virtual std::vector<glm::vec3> &getVertexArray() = 0;
        virtual std::vector<glm::vec4> &getColorArray() = 0;
        virtual std::vector<glm::vec3> &getNormalArray() = 0;

        virtual std::vector<glm::vec4> &getBoneIndexArray() = 0;
        virtual std::vector<glm::vec4> &getBoneWeightArray() = 0;

        virtual void dirty() = 0;


    };

}


#endif /* INCLUDE_ODCORE_RENDER_GEOMETRY_H_ */
