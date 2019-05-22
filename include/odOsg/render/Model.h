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

        virtual size_t getGeometryCount() override;
        virtual odRender::Geometry *getGeometry(size_t index) override;

        virtual odRender::Geometry *createNewGeometry() override;


    private:

        std::vector<od::RefPtr<Geometry>> mGeometries;

        osg::ref_ptr<osg::Geode> mGeode;

    };

}


#endif /* INCLUDE_ODOSG_RENDER_MODEL_H_ */
