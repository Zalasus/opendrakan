/*
 * Model.h
 *
 *  Created on: May 15, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_RENDER_MODEL_H_
#define INCLUDE_ODOSG_RENDER_MODEL_H_

#include <vector>

#include <osg/Geode>

#include <odCore/render/Model.h>

#include <odOsg/render/Geometry.h>

namespace odOsg
{

    class Model : public odRender::Model
    {
    public:

        Model();

        inline osg::Geode *getGeode() { return mGeode; }
        inline void setHasSharedVertexArrays(bool b) { mHasSharedVertexArrays = b; }

        virtual size_t getGeometryCount() override;
        virtual std::shared_ptr<odRender::Geometry> getGeometry(size_t index) override;
        virtual void addGeometry(std::shared_ptr<odRender::Geometry> g) override;
        virtual void removeGeometry(std::shared_ptr<odRender::Geometry> g) override;

        virtual bool hasSharedVertexArrays() override;

        virtual void setLightingMode(odRender::LightingMode lm) override;


    private:

        std::vector<std::shared_ptr<Geometry>> mGeometries;

        osg::ref_ptr<osg::Geode> mGeode;

        bool mHasSharedVertexArrays;

    };

}


#endif /* INCLUDE_ODOSG_RENDER_MODEL_H_ */
