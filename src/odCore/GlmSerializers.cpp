/*
 * GlmSerializers.cpp
 *
 *  Created on: 4 Nov 2018
 *      Author: zal
 */

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

#include <odCore/DataStream.h>

namespace od
{

    template <>
    DataReader &DataReader::operator >> <glm::vec2>(glm::vec2 &v)
    {
        float x;
        float y;

        (*this) >> x >> y;

        v.x = x;
        v.y = y;

        return *this;
    }

    template <>
    DataReader &DataReader::operator >> <glm::vec3>(glm::vec3 &v)
    {
        float x;
        float y;
        float z;

        (*this) >> x >> y >> z;

        v.x = x;
        v.y = y;
        v.z = z;

        return *this;
    }

    template <>
    DataReader &DataReader::operator >> <glm::quat>(glm::quat &q)
    {
        float x;
        float y;
        float z;
        float w;

        (*this) >> x >> y >> z >> w;

        q.x = x;
        q.y = y;
        q.z = z;
        q.w = w;

        return *this;
    }

    template <>
    DataReader &DataReader::operator >> <glm::mat3>(glm::mat3 &m)
    {
        float l[9];
        for(size_t i = 0; i < sizeof(l)/sizeof(float); ++i)
        {
            (*this) >> l[i];
        }

        m = glm::mat3(l[0], l[1], l[2],
                      l[3], l[4], l[5],
                      l[6], l[7], l[8]); // TODO: do i need to transpose this?

        return *this;
    }

    template <>
    DataReader &DataReader::operator >> <glm::mat4>(glm::mat4 &m)
    {
        float l[9]; // linear thingy
        glm::vec3 t; // offset

        for(size_t i = 0; i < sizeof(l)/sizeof(float); ++i)
        {
            (*this) >> l[i];
        }

        (*this) >> t;

        m = glm::mat4(l[0], l[1], l[2], 0,
                      l[3], l[4], l[5], 0,
                      l[6], l[7], l[8], 0,
                      t[0], t[1], t[2], 1); // TODO: do i need to transpose this?

        return *this;
    }

}
