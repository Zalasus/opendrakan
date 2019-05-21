/*
 * OsgArray.h
 *
 *  Created on: May 21, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_RENDER_OSGARRAY_H_
#define INCLUDE_ODOSG_RENDER_OSGARRAY_H_

#include <osg/Array>

#include <glm/vec2.hpp>

#include <odCore/render/Array.h>

#include <odOsg/GlmAdapter.h>

namespace odOsg
{

    template <typename _OdType, typename _OsgArrayType>
    class OsgTVecArrayAccessor : public odRender::TVecArrayAccessor<_OdType>
    {
    public:

        OsgTVecArrayAccessor(_OsgArrayType *osgArray)
        : mOsgArray(osgArray)
        , mAcquired(false)
        {
        }

        virtual ~OsgTVecArrayAccessor()
        {
            release();
        }

        virtual odRender::Array<_ElementType> &getArray() override
        {
            if(!mAcquired)
            {
                throw od::Exception("Accessed array of array accessor without acquiring it first");
            }

            return mArray;
        }

        virtual void acquire() override
        {
            if(mAcquired)
            {
                return;
            }

            mArray.resize(mOsgArray->size());

            for(size_t i = 0; i < mOsgArray->size(); ++i)
            {
                mArray[i] = GlmAdapter::toGlm((*mOsgArray)[i]);
            }

            mAcquired = true;
        }

        virtual void release() override
        {
            if(!mAcquired)
            {
                return;
            }

            mOsgArray->resize(mArray.size());

            for(size_t i = 0; i < mArray->size(); ++i)
            {
                (*mOsgArray)[i] = GlmAdapter::toOsg(mArray[i]);
            }

            mArray.clear();
            mArray.shrink_to_fit();

            mAcquired = false;
        }


    private:

        odRender::Array<_ElementType> mArray;
        osg::ref_ptr<_OsgArrayType> mOsgArray;

        bool mAcquired;

    };

    typedef OsgTVecArrayAccessor<glm::vec2, osg::Vec2Array> OsgVec2ArrayAccessor;
    typedef OsgTVecArrayAccessor<glm::vec3, osg::Vec3Array> OsgVec3ArrayAccessor;
    typedef OsgTVecArrayAccessor<glm::vec4, osg::Vec4Array> OsgVec4ArrayAccessor;

}


#endif /* INCLUDE_ODOSG_RENDER_OSGARRAY_H_ */
