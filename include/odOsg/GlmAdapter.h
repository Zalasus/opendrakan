/*
 * GlmAdapter.h
 *
 *  Created on: 12 Nov 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_GLMADAPTER_H_
#define INCLUDE_ODOSG_GLMADAPTER_H_

#include <vector>

#include <osg/Vec2>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Quat>
#include <osg/Array>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/quaternion.hpp>

namespace odOsg
{

    class GlmAdapter
    {
    public:

        inline static osg::Vec2 toOsg(const glm::vec2 &v)
        {
            return osg::Vec2(v.x, v.y);
        }

        inline static osg::Vec3 toOsg(const glm::vec3 &v)
        {
            return osg::Vec3(v.x, v.y, v.z);
        }

        inline static osg::Vec4 toOsg(const glm::vec4 &v)
        {
            return osg::Vec4(v.x, v.y, v.z, v.w);
        }

        inline static osg::Quat toOsg(const glm::quat &q)
        {
            return osg::Quat(q.x, q.y, q.z, q.w);
        }

        inline static osg::Matrix toOsg(const glm::mat4 &m)
        {
            osg::Matrix om;
            for(size_t r = 0; r < 3; ++r)
            {
                for(size_t c = 0; c < 3; ++c)
                {
                    om(r, c) = m[c][r];
                }
            }

            return om;
        }


        inline static glm::vec2 toGlm(const osg::Vec2 &v)
        {
            return glm::vec2(v.x(), v.y());
        }

        inline static glm::vec3 toGlm(const osg::Vec3 &v)
        {
            return glm::vec3(v.x(), v.y(), v.z());
        }

        inline static glm::vec4 toGlm(const osg::Vec4 &v)
        {
            return glm::vec4(v.x(), v.y(), v.z(), v.w());
        }

        inline static glm::quat toGlm(const osg::Quat &q)
        {
            return glm::quat(q.w(), q.x(), q.y(), q.z()); // yep, order is correct. GLM defines this differently. easy to miss
        }


        template <typename _OsgArrayType, typename _GlmVectorType>
        static _OsgArrayType *convertToOsgArray(const std::vector<_GlmVectorType> &v)
        {
            osg::ref_ptr<_OsgArrayType> osgArray = new _OsgArrayType(v.size());
            for(size_t i = 0; i < v.size(); ++i)
            {
                (*osgArray)[i] = GlmAdapter::toOsg(v[i]);
            }

            return osgArray.release();
        }


    };

}

#endif /* INCLUDE_ODOSG_GLMADAPTER_H_ */
