/*
 * Geometry.cpp
 *
 *  Created on: 14 Nov 2018
 *      Author: zal
 */

#include <odOsg/render/Geometry.h>

#include <odCore/Exception.h>
#include <odCore/Downcast.h>

#include <odOsg/GlmAdapter.h>
#include <odOsg/Constants.h>

#include <odOsg/render/Texture.h>
#include <odOsg/render/OsgArrayAccessHandler.h>

namespace odOsg
{

    /**
     * @brief Custom array accessor, adjusting the binding of color attributes.
     *
     * This will set the color array's binding to overall if it contains only one vector.
     */
    class ColorArrayAccessor : public OsgVec4ArrayAccessHandler
    {
    public:

        ColorArrayAccessor(osg::Vec4Array *array)
        : OsgVec4ArrayAccessHandler(array)
        {
        }

        virtual void release() override
        {
            OsgVec4ArrayAccessHandler::release();

            osg::Array::Binding binding = mOsgArray->size() == 1 ? osg::Array::BIND_OVERALL : osg::Array::BIND_PER_VERTEX;
            mOsgArray->setBinding(binding);
        }

    };

    class IndexArrayAccessHandler : public odRender::ArrayAccessHandler<int32_t>
    {
    public:

        IndexArrayAccessHandler(osg::DrawElements *de)
        {

        }

        virtual odRender::Array<_ElementType> &getArray() override
        {
            return mArray;
        }

        virtual void acquire() override
        {

        }

        virtual void release() override
        {

        }


    private:

        odRender::Array<_ElementType> mArray;

    };


    Geometry::Geometry()
    : mGeometry(new osg::Geometry)
    , mOsgVertexArray(new osg::Vec3Array)
    , mOsgColorArray(new osg::Vec4Array)
    , mOsgNormalArray(new osg::Vec3Array)
    , mOsgTextureCoordArray(new osg::Vec2Array)
    {
        mGeometry->setUseDisplayList(false);
        mGeometry->setUseVertexBufferObjects(true);

        mGeometry->setVertexArray(mOsgVertexArray);
        mGeometry->setColorArray(mOsgColorArray);
        mGeometry->setNormalArray(mOsgNormalArray);
        mGeometry->setTexCoordArray(0, mOsgTextureCoordArray);
    }

    Geometry::Geometry(osg::Geometry *geometry)
    : mGeometry(geometry)
    {
        if(mGeometry == nullptr)
        {
            throw od::Exception("Got nullptr geometry");
        }

        mOsgVertexArray = dynamic_cast<osg::Vec3Array*>(mGeometry->getVertexArray());
        if(mOsgVertexArray == nullptr)
        {
            throw od::Exception("Passed geometry had no valid vertex array");
        }

        mOsgColorArray = dynamic_cast<osg::Vec4Array*>(mGeometry->getColorArray());
        if(mOsgColorArray == nullptr)
        {
            throw od::Exception("Passed geometry had no valid color array");
        }

        mOsgNormalArray = dynamic_cast<osg::Vec3Array*>(mGeometry->getNormalArray());
        if(mOsgNormalArray == nullptr)
        {
            throw od::Exception("Passed geometry had no valid normal array");
        }

        mOsgTextureCoordArray = dynamic_cast<osg::Vec2Array*>(mGeometry->getTexCoordArray(0));
        if(mOsgTextureCoordArray == nullptr)
        {
            throw od::Exception("Passed geometry had no valid texture coordinate array");
        }

        mOsgBoneIndexArray = dynamic_cast<osg::Vec4Array*>(mGeometry->getVertexAttribArray(Constants::ATTRIB_INFLUENCE_LOCATION));
        mOsgBoneWeightArray = dynamic_cast<osg::Vec4Array*>(mGeometry->getVertexAttribArray(Constants::ATTRIB_WEIGHT_LOCATION));
        if((mOsgBoneIndexArray == nullptr) != (mOsgBoneWeightArray == nullptr))
        {
            throw od::Exception("Passed geometry had one of weight/index array, but not the other. Must have either none or both");
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

        }else if(!b && hasBoneInfo())
        {
            mGeometry->setVertexAttribArray(Constants::ATTRIB_INFLUENCE_LOCATION, nullptr);
            mOsgBoneIndexArray = nullptr;

            mGeometry->setVertexAttribArray(Constants::ATTRIB_WEIGHT_LOCATION, nullptr);
            mOsgBoneWeightArray = nullptr;
        }
    }

    bool Geometry::hasBoneInfo() const
    {
        return mOsgBoneIndexArray != nullptr && mOsgBoneWeightArray != nullptr;
    }

    std::unique_ptr<odRender::ArrayAccessHandler<glm::vec3>> Geometry::getVertexArrayAccessHandler()
    {
        return std::make_unique<OsgVec3ArrayAccessHandler>(mOsgVertexArray);
    }

    std::unique_ptr<odRender::ArrayAccessHandler<glm::vec4>> Geometry::getColorArrayAccessHandler()
    {
        return std::make_unique<ColorArrayAccessor>(mOsgColorArray);
    }

    std::unique_ptr<odRender::ArrayAccessHandler<glm::vec3>> Geometry::getNormalArrayAccessHandler()
    {
        return std::make_unique<OsgVec3ArrayAccessHandler>(mOsgNormalArray);
    }

    std::unique_ptr<odRender::ArrayAccessHandler<glm::vec2>> Geometry::getTextureCoordArrayAccessHandler()
    {
        return std::make_unique<OsgVec2ArrayAccessHandler>(mOsgTextureCoordArray);
    }

    std::unique_ptr<odRender::ArrayAccessHandler<int32_t>> Geometry::getIndexArrayAccessHandler()
    {
        throw od::UnsupportedException("Accessing index array is unsupported right now");
    }

    std::unique_ptr<odRender::ArrayAccessHandler<glm::vec4>> Geometry::getBoneIndexArrayAccessHandler()
    {
        if(mOsgBoneIndexArray == nullptr)
        {
            return nullptr;
        }

        return std::make_unique<OsgVec4ArrayAccessHandler>(mOsgBoneIndexArray);
    }

    std::unique_ptr<odRender::ArrayAccessHandler<glm::vec4>> Geometry::getBoneWeightArrayAccessHandler()
    {
        if(mOsgBoneWeightArray == nullptr)
        {
            return nullptr;
        }

        return std::make_unique<OsgVec4ArrayAccessHandler>(mOsgBoneWeightArray);
    }

    void Geometry::setTexture(odRender::Texture *texture)
    {
        mTexture = od::confident_downcast<Texture>(texture);
    }

    bool Geometry::usesIndexedRendering()
    {
        return true;
    }

    odRender::PrimitiveType Geometry::getPrimitiveType()
    {
        return odRender::PrimitiveType::TRIANGLES;
    }
}
