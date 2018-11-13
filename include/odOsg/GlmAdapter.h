/*
 * GlmAdapter.h
 *
 *  Created on: 12 Nov 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_GLMADAPTER_H_
#define INCLUDE_ODOSG_GLMADAPTER_H_

#include <vector>

#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Quat>
#include <osg/Array>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/quaternion.hpp>

namespace odOsg
{

    class GlmAdapter
    {
    public:

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
