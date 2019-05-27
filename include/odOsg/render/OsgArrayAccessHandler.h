/*
 * OsgArray.h
 *
 *  Created on: May 21, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_RENDER_OSGARRAYACCESSHANDLER_H_
#define INCLUDE_ODOSG_RENDER_OSGARRAYACCESSHANDLER_H_

#include <osg/Array>

#include <glm/vec2.hpp>

#include <odCore/render/Array.h>

#include <odOsg/GlmAdapter.h>

namespace odOsg
{

    template <typename _OdType, typename _OsgArrayType>
    class OsgTVecArrayAccessHandler : public odRender::ArrayAccessHandler<_OdType>
    {
    public:

        OsgTVecArrayAccessHandler(_OsgArrayType *osgArray)
        : mOsgArray(osgArray)
        , mAcquired(false)
        {
        }

        virtual ~OsgTVecArrayAccessHandler()
        {
            release();
        }

        virtual odRender::Array<_OdType> &getArray() override
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

            for(size_t i = 0; i < mArray.size(); ++i)
            {
                (*mOsgArray)[i] = GlmAdapter::toOsg(mArray[i]);
            }

            mArray.clear();
            mArray.shrink_to_fit();

            mOsgArray->dirty();

            mAcquired = false;
        }


    protected:

        odRender::Array<_OdType> mArray;
        osg::ref_ptr<_OsgArrayType> mOsgArray;

        bool mAcquired;

    };

    // TODO: only forward specialize these and put explicit instantiation into source file
    typedef OsgTVecArrayAccessHandler<glm::vec2, osg::Vec2Array> OsgVec2ArrayAccessHandler;
    typedef OsgTVecArrayAccessHandler<glm::vec3, osg::Vec3Array> OsgVec3ArrayAccessHandler;
    typedef OsgTVecArrayAccessHandler<glm::vec4, osg::Vec4Array> OsgVec4ArrayAccessHandler;
}


#endif /* INCLUDE_ODOSG_RENDER_OSGARRAYACCESSHANDLER_H_ */
