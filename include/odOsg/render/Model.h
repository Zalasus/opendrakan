/*
 * Model.h
 *
 *  Created on: May 15, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_RENDER_MODEL_H_
#define INCLUDE_ODOSG_RENDER_MODEL_H_

#include <odCore/render/Model.h>

namespace odOsg
{

    class Model : public odRender::Model
    {
    public:

        virtual size_t getGeometryCount() override;
        virtual odRender::Geometry *getGeometry(size_t index) override;

        virtual odRender::Geometry *createNewGeometry() override;

    };

}


#endif /* INCLUDE_ODOSG_RENDER_MODEL_H_ */
