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
#include <osg/Geode>

namespace odOsg
{

    class GeometrySegment : public odRender::GeometrySegment
    {

    };

    class Geometry : public odRender::Geometry
    {
    public:

        Geometry();

        inline osg::Geode *getNode() { return mGeode; }

        virtual std::vector<glm::vec3> &getVertexArray() override;
        virtual std::vector<glm::vec4> &getColorArray() override;
        virtual std::vector<glm::vec3> &getNormalArray() override;
        virtual std::vector<glm::vec2> &getTextureCoordArray() override;
        virtual void setHasBoneInfo(bool b) override;
        virtual bool hasBoneInfo() const override;
        virtual std::vector<glm::vec4> &getBoneIndexArray() override;
        virtual std::vector<glm::vec4> &getBoneWeightArray() override;
        virtual void notifyVertexDataChanged(bool lastUpdate) override;
        virtual odRender::GeometrySegment *addSegment(size_t indexCount, odDb::Texture *texture) override;


    private:

        osg::ref_ptr<osg::Geode> mGeode;
        osg::ref_ptr<osg::Vec3Array> mOsgVertexArray;
        osg::ref_ptr<osg::Vec4Array> mOsgColorArray;
        osg::ref_ptr<osg::Vec3Array> mOsgNormalArray;
        osg::ref_ptr<osg::Vec2Array> mOsgTextureCoordArray;
        osg::ref_ptr<osg::Vec4Array> mOsgBoneIndexArray;
        osg::ref_ptr<osg::Vec4Array> mOsgBoneWeightArray;

        bool mArrayDataValid;
        std::vector<glm::vec3> mVertexArray;
        std::vector<glm::vec4> mColorArray;
        std::vector<glm::vec3> mNormalArray;
        std::vector<glm::vec2> mTextureCoordArray;

        bool mHasBoneInfo;
        std::vector<glm::vec4> mBoneIndexArray;
        std::vector<glm::vec4> mBoneWeightArray;
    };

}

#endif /* INCLUDE_ODOSG_GEOMETRY_H_ */
