/**
 * @file ModelImpl.h
 * @date Jan 26, 2020
 * @author zal
 */

#ifndef INCLUDE_ODGL_MODELIMPL_H_
#define INCLUDE_ODGL_MODELIMPL_H_

#include <odCore/render/Model.h>

namespace odGl
{

    class ModelImpl : public odRender::Model
    {
    public:

        virtual size_t getGeometryCount() override;
        virtual odRender::Geometry *getGeometry(size_t index) override;
        virtual void addGeometry(odRender::Geometry *g) override;
        virtual void removeGeometry(odRender::Geometry *g) override;

        virtual bool hasSharedVertexArrays() override;

        virtual void setLightingMode(odRender::LightingMode lm) override;

    };

}


#endif /* INCLUDE_ODGL_MODELIMPL_H_ */
