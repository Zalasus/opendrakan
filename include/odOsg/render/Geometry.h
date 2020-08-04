/*
 * Geometry.h
 *
 *  Created on: 13 Nov 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_GEOMETRY_H_
#define INCLUDE_ODOSG_GEOMETRY_H_

#include <odCore/render/Geometry.h>

#include <osg/Node>
#include <osg/Geometry>

#include <odOsg/render/Texture.h>
#include <odOsg/render/OsgArrayAccessHandler.h>

namespace odOsg
{
    /**
     * @brief Custom array access handler, adjusting the binding of color attributes.
     *
     * This will set the color array's binding to overall if it contains only one vector.
     */
    class ColorArrayAccessHandler : public OsgVec4ArrayAccessHandler
    {
    public:

        explicit ColorArrayAccessHandler(osg::Vec4Array *array);

        virtual void release(bool writeback) override;
    };

    /**
     * @brief Custom access handler for the vertex array. Will update element count if not using indexed rendering.
     */
    class VertexArrayAccessHandler : public OsgVec3ArrayAccessHandler
    {
    public:

        VertexArrayAccessHandler(osg::Vec3Array *array, osg::DrawArrays *drawArrays);

        inline void setDrawArrays(osg::DrawArrays *drawArrays) { mDrawArrays = drawArrays; }

        virtual void release(bool writeback) override;


    private:

        osg::ref_ptr<osg::DrawArrays> mDrawArrays;

    };


    class Geometry : public odRender::Geometry
    {
    public:

        /**
         * @brief Constructs a Geometry which will create it's own osg::Geometry and arrays.
         */
        Geometry(odRender::PrimitiveType primitiveType, bool indexed);

        /**
         * @brief Constructs a Geometry using a provided osg::Geometry and it's arrays.
         */
        Geometry(osg::Geometry *geode);

        inline osg::Geometry *getOsgGeometry() { return mGeometry; }

        virtual void setHasBoneInfo(bool b) override;
        virtual bool hasBoneInfo() const override;
        virtual odRender::ArrayAccessHandler<glm::vec4> &getBoneIndexArrayAccessHandler() override;
        virtual odRender::ArrayAccessHandler<glm::vec4> &getBoneWeightArrayAccessHandler() override;

        virtual odRender::ArrayAccessHandler<glm::vec3> &getVertexArrayAccessHandler() override;
        virtual odRender::ArrayAccessHandler<glm::vec4> &getColorArrayAccessHandler() override;
        virtual odRender::ArrayAccessHandler<glm::vec3> &getNormalArrayAccessHandler() override;
        virtual odRender::ArrayAccessHandler<glm::vec2> &getTextureCoordArrayAccessHandler() override;

        virtual odRender::ArrayAccessHandler<int32_t> &getIndexArrayAccessHandler() override;

        virtual void setTexture(std::shared_ptr<odRender::Texture> texture) override;

        virtual bool usesIndexedRendering() override;
        virtual odRender::PrimitiveType getPrimitiveType() override;


    private:

        void _buildOsgObjects();

        odRender::PrimitiveType mPrimitiveType;
        bool mIndexed;

        osg::ref_ptr<osg::Geometry> mGeometry;
        osg::ref_ptr<osg::Vec3Array> mOsgVertexArray;
        osg::ref_ptr<osg::Vec4Array> mOsgColorArray;
        osg::ref_ptr<osg::Vec3Array> mOsgNormalArray;
        osg::ref_ptr<osg::Vec2Array> mOsgTextureCoordArray;

        osg::ref_ptr<osg::Vec4Array> mOsgBoneIndexArray;
        osg::ref_ptr<osg::Vec4Array> mOsgBoneWeightArray;

        osg::ref_ptr<osg::PrimitiveSet> mPrimitiveSet;

        std::shared_ptr<Texture> mTexture;

        VertexArrayAccessHandler  mVertexArrayAccessHandler;
        ColorArrayAccessHandler   mColorArrayAccessHandler;
        OsgVec3ArrayAccessHandler mNormalArrayAccessHandler;
        OsgVec2ArrayAccessHandler mTextureCoordArrayAccessHandler;
        OsgVec4ArrayAccessHandler mBoneIndexArrayAccessHandler;
        OsgVec4ArrayAccessHandler mBoneWeightArrayAccessHandler;
    };

}

#endif /* INCLUDE_ODOSG_GEOMETRY_H_ */
