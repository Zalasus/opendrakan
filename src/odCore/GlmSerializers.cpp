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
#include <glm/mat3x4.hpp>
#include <glm/matrix.hpp>

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
        // riot engine uses row-major storage -> read as column vectors, then transpose
        for(size_t i = 0; i < 3; ++i)
        {
            (*this) >> m[i];
        }

        m = glm::transpose(m);

        return *this;
    }

    template <>
    DataReader &DataReader::operator >> <glm::mat3x4>(glm::mat3x4 &m)
    {
        glm::mat3 mat;
        (*this) >> mat;

        glm::vec3 translation;
        (*this) >> translation;

        m = glm::mat3x4(mat);
        m[0].w = translation.x;
        m[1].w = translation.y;
        m[2].w = translation.z;

        return *this;
    }

}
