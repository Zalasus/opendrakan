/*
 * Geometry.cpp
 *
 *  Created on: 14 Nov 2018
 *      Author: zal
 */

#include <odOsg/render/Geometry.h>

#include <odCore/Panic.h>
#include <odCore/Downcast.h>

#include <odOsg/GlmAdapter.h>
#include <odOsg/Constants.h>

#include <odOsg/render/Texture.h>
#include <odOsg/render/OsgArrayAccessHandler.h>

namespace odOsg
{

    ColorArrayAccessHandler::ColorArrayAccessHandler(osg::Vec4Array *array)
    : OsgVec4ArrayAccessHandler(array)
    {
    }

    void ColorArrayAccessHandler::release(bool writeback)
    {
        OsgVec4ArrayAccessHandler::release(writeback);

        osg::Array::Binding binding = mOsgArray->size() == 1 ? osg::Array::BIND_OVERALL : osg::Array::BIND_PER_VERTEX;
        mOsgArray->setBinding(binding);
    }


    VertexArrayAccessHandler::VertexArrayAccessHandler(osg::Vec3Array *array, osg::DrawArrays *drawArrays)
    : OsgVec3ArrayAccessHandler(array)
    , mDrawArrays(drawArrays)
    {
    }

    void VertexArrayAccessHandler::release(bool writeback)
    {
        OsgVec3ArrayAccessHandler::release(writeback);

        if(mDrawArrays != nullptr)
        {
            mDrawArrays->setCount(mOsgArray->size());
        }
    }


    Geometry::Geometry(odRender::PrimitiveType primitiveType, bool indexed)
    : mPrimitiveType(primitiveType)
    , mIndexed(indexed)
    , mGeometry(new osg::Geometry)
    , mOsgVertexArray(new osg::Vec3Array)
    , mOsgColorArray(new osg::Vec4Array)
    , mOsgNormalArray(new osg::Vec3Array)
    , mOsgTextureCoordArray(new osg::Vec2Array)
    , mVertexArrayAccessHandler(mOsgVertexArray, nullptr)
    , mColorArrayAccessHandler(mOsgColorArray)
    , mNormalArrayAccessHandler(mOsgNormalArray)
    , mTextureCoordArrayAccessHandler(mOsgTextureCoordArray)
    , mBoneIndexArrayAccessHandler(nullptr)
    , mBoneWeightArrayAccessHandler(nullptr)
    , mIndexArrayAccessHandler(nullptr)
    {
        mGeometry->setUseDisplayList(false);
        mGeometry->setUseVertexBufferObjects(true);

        mGeometry->setVertexArray(mOsgVertexArray);
        mGeometry->setColorArray(mOsgColorArray);
        mGeometry->setNormalArray(mOsgNormalArray);
        mGeometry->setTexCoordArray(0, mOsgTextureCoordArray);

        GLenum glPrimitiveType;
        switch(primitiveType)
        {
        case odRender::PrimitiveType::LINES:
            glPrimitiveType = GL_LINES;
            break;

        case odRender::PrimitiveType::TRIANGLES:
            glPrimitiveType = GL_TRIANGLES;
            break;

        case odRender::PrimitiveType::TRIANGLE_FAN:
            glPrimitiveType = GL_TRIANGLE_FAN;
            break;

        case odRender::PrimitiveType::TRIANGLE_STRIP:
            glPrimitiveType = GL_TRIANGLE_STRIP;
            break;

        default:
            OD_PANIC() << "Unsupported or unknown primitive type";
        }

        if(indexed)
        {
            auto drawElements = new osg::DrawElementsUInt(glPrimitiveType);
            mPrimitiveSet = drawElements;
            mIndexArrayAccessHandler.setArray(drawElements);

        }else
        {
            osg::ref_ptr<osg::DrawArrays> drawArrays = new osg::DrawArrays(glPrimitiveType);
            mVertexArrayAccessHandler.setDrawArrays(drawArrays);
            mPrimitiveSet = drawArrays;
        }
        mGeometry->addPrimitiveSet(mPrimitiveSet);

        setTexture(nullptr);
    }

    Geometry::Geometry(osg::Geometry *geometry)
    : mGeometry(geometry)
    , mVertexArrayAccessHandler(nullptr, nullptr)
    , mColorArrayAccessHandler(nullptr)
    , mNormalArrayAccessHandler(nullptr)
    , mTextureCoordArrayAccessHandler(nullptr)
    , mBoneIndexArrayAccessHandler(nullptr)
    , mBoneWeightArrayAccessHandler(nullptr)
    , mIndexArrayAccessHandler(nullptr)
    {
        OD_CHECK_ARG_NONNULL(geometry);

        mOsgVertexArray = dynamic_cast<osg::Vec3Array*>(mGeometry->getVertexArray());
        if(mOsgVertexArray == nullptr)
        {
            OD_PANIC() << "Passed geometry had no valid vertex array";
        }
        mVertexArrayAccessHandler.setArray(mOsgVertexArray);

        mOsgColorArray = dynamic_cast<osg::Vec4Array*>(mGeometry->getColorArray());
        if(mOsgColorArray == nullptr)
        {
            OD_PANIC() << "Passed geometry had no valid color array";
        }
        mColorArrayAccessHandler.setArray(mOsgColorArray);

        mOsgNormalArray = dynamic_cast<osg::Vec3Array*>(mGeometry->getNormalArray());
        if(mOsgNormalArray == nullptr)
        {
            OD_PANIC() << "Passed geometry had no valid normal array";
        }
        mNormalArrayAccessHandler.setArray(mOsgNormalArray);

        mOsgTextureCoordArray = dynamic_cast<osg::Vec2Array*>(mGeometry->getTexCoordArray(0));
        if(mOsgTextureCoordArray == nullptr)
        {
            OD_PANIC() << "Passed geometry had no valid texture coordinate array";
        }
        mTextureCoordArrayAccessHandler.setArray(mOsgTextureCoordArray);

        mOsgBoneIndexArray = dynamic_cast<osg::Vec4Array*>(mGeometry->getVertexAttribArray(Constants::ATTRIB_INFLUENCE_LOCATION));
        mOsgBoneWeightArray = dynamic_cast<osg::Vec4Array*>(mGeometry->getVertexAttribArray(Constants::ATTRIB_WEIGHT_LOCATION));
        if((mOsgBoneIndexArray == nullptr) != (mOsgBoneWeightArray == nullptr))
        {
            OD_PANIC() << "Passed geometry had one of weight/index array, but not the other. Must have either none or both";
        }
        mBoneIndexArrayAccessHandler.setArray(mOsgBoneIndexArray);
        mBoneWeightArrayAccessHandler.setArray(mOsgBoneWeightArray);

        if(mGeometry->getNumPrimitiveSets() == 0)
        {
            OD_PANIC() << "Passed geometry had no primitive set";
        }
        mPrimitiveSet = mGeometry->getPrimitiveSet(0);

        auto drawArrays = dynamic_cast<osg::DrawArrays*>(mPrimitiveSet.get());
        if(drawArrays != nullptr)
        {
            mVertexArrayAccessHandler.setDrawArrays(drawArrays);
            mIndexed = false;

        }else
        {
            auto drawElements = dynamic_cast<osg::DrawElementsUInt*>(mPrimitiveSet.get());
            if(drawElements != nullptr)
            {
                mIndexArrayAccessHandler.setArray(drawElements);
                mIndexed = true;
            }
        }

        switch(mPrimitiveSet->getMode())
        {
        case GL_LINES:
            mPrimitiveType = odRender::PrimitiveType::LINES;
            break;

        case GL_TRIANGLES:
            mPrimitiveType = odRender::PrimitiveType::TRIANGLES;
            break;

        case GL_TRIANGLE_FAN:
            mPrimitiveType = odRender::PrimitiveType::TRIANGLE_FAN;
            break;

        case GL_TRIANGLE_STRIP:
            mPrimitiveType = odRender::PrimitiveType::TRIANGLE_STRIP;
            break;

        default:
            OD_PANIC() << "Unsupported or unknown primitive type";
        }
    }

    void Geometry::setHasBoneInfo(bool b)
    {
        if(b && !hasBoneInfo())
        {
            mOsgBoneIndexArray = new osg::Vec4Array;
            mOsgBoneIndexArray->setBinding(osg::Array::BIND_PER_VERTEX);
            mGeometry->setVertexAttribArray(Constants::ATTRIB_INFLUENCE_LOCATION, mOsgBoneIndexArray);

            mOsgBoneWeightArray = new osg::Vec4Array;
            mOsgBoneWeightArray->setBinding(osg::Array::BIND_PER_VERTEX);
            mGeometry->setVertexAttribArray(Constants::ATTRIB_WEIGHT_LOCATION, mOsgBoneWeightArray);

            mBoneIndexArrayAccessHandler.setArray(mOsgBoneIndexArray);
            mBoneWeightArrayAccessHandler.setArray(mOsgBoneWeightArray);

        }else if(!b && hasBoneInfo())
        {
            mGeometry->setVertexAttribArray(Constants::ATTRIB_INFLUENCE_LOCATION, nullptr);
            mOsgBoneIndexArray = nullptr;

            mGeometry->setVertexAttribArray(Constants::ATTRIB_WEIGHT_LOCATION, nullptr);
            mOsgBoneWeightArray = nullptr;

            mBoneIndexArrayAccessHandler.setArray(nullptr);
            mBoneWeightArrayAccessHandler.setArray(nullptr);
        }
    }

    bool Geometry::hasBoneInfo() const
    {
        return mOsgBoneIndexArray != nullptr && mOsgBoneWeightArray != nullptr;
    }

    odRender::ArrayAccessHandler<glm::vec3> &Geometry::getVertexArrayAccessHandler()
    {
        return mVertexArrayAccessHandler;
    }

    odRender::ArrayAccessHandler<glm::vec4> &Geometry::getColorArrayAccessHandler()
    {
        return mColorArrayAccessHandler;
    }

    odRender::ArrayAccessHandler<glm::vec3> &Geometry::getNormalArrayAccessHandler()
    {
        return mNormalArrayAccessHandler;
    }

    odRender::ArrayAccessHandler<glm::vec2> &Geometry::getTextureCoordArrayAccessHandler()
    {
        return mTextureCoordArrayAccessHandler;
    }

    odRender::ArrayAccessHandler<int32_t> &Geometry::getIndexArrayAccessHandler()
    {
        if(!mIndexed)
        {
            OD_PANIC() << "Not indexed";
        }

        return mIndexArrayAccessHandler;
    }

    odRender::ArrayAccessHandler<glm::vec4> &Geometry::getBoneIndexArrayAccessHandler()
    {
        return mBoneIndexArrayAccessHandler;
    }

    odRender::ArrayAccessHandler<glm::vec4> &Geometry::getBoneWeightArrayAccessHandler()
    {
        return mBoneWeightArrayAccessHandler;
    }

    void Geometry::setTexture(std::shared_ptr<odRender::Texture> texture)
    {
        mTexture = od::confident_downcast<Texture>(texture);

        osg::StateSet *ss = mGeometry->getOrCreateStateSet();
        if(mTexture != nullptr)
        {
            ss->setDefine("TEXTURE");
            ss->setTextureAttribute(0, mTexture->getOsgTexture(), osg::StateAttribute::ON);

        }else
        {
            ss->removeDefine("TEXTURE");
            ss->setTextureAttribute(0, nullptr, osg::StateAttribute::OFF);
        }
    }

    bool Geometry::usesIndexedRendering()
    {
        return mIndexed;
    }

    odRender::PrimitiveType Geometry::getPrimitiveType()
    {
        return mPrimitiveType;
    }
}
