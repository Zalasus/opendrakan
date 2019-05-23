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

namespace odOsg
{

    class Geometry : public odRender::Geometry
    {
    public:

        /**
         * @brief Constructs a Geometry which will create it's own osg::Geometry and arrays.
         */
        Geometry();

        /**
         * @brief Constructs a Geometry using a provided osg::Geometry and it's arrays.
         */
        Geometry(osg::Geometry *geode);

        inline osg::Geometry *getOsgGeometry() { return mGeometry; }

        virtual void setHasBoneInfo(bool b) override;
        virtual bool hasBoneInfo() const override;
        virtual std::unique_ptr<odRender::ArrayAccessHandler<glm::vec4>> getBoneIndexArrayAccessHandler() override;
        virtual std::unique_ptr<odRender::ArrayAccessHandler<glm::vec4>> getBoneWeightArrayAccessHandler() override;

        virtual std::unique_ptr<odRender::ArrayAccessHandler<glm::vec3>> getVertexArrayAccessHandler() override;
        virtual std::unique_ptr<odRender::ArrayAccessHandler<glm::vec4>> getColorArrayAccessHandler() override;
        virtual std::unique_ptr<odRender::ArrayAccessHandler<glm::vec3>> getNormalArrayAccessHandler() override;
        virtual std::unique_ptr<odRender::ArrayAccessHandler<glm::vec2>> getTextureCoordArrayAccessHandler() override;

        virtual std::unique_ptr<odRender::ArrayAccessHandler<int32_t>> getIndexArrayAccessHandler() override;

        virtual void setTexture(odRender::Texture *texture) override;


    private:

        void _buildOsgObjects();

        osg::ref_ptr<osg::Geometry> mGeometry;
        osg::ref_ptr<osg::Vec3Array> mOsgVertexArray;
        osg::ref_ptr<osg::Vec4Array> mOsgColorArray;
        osg::ref_ptr<osg::Vec3Array> mOsgNormalArray;
        osg::ref_ptr<osg::Vec2Array> mOsgTextureCoordArray;

        osg::ref_ptr<osg::Vec4Array> mOsgBoneIndexArray;
        osg::ref_ptr<osg::Vec4Array> mOsgBoneWeightArray;

        std::vector<od::RefPtr<Texture>> mTextures;
    };

}

#endif /* INCLUDE_ODOSG_GEOMETRY_H_ */
