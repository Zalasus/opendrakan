/*
 * Model.h
 *
 *  Created on: May 15, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_MODEL_H_
#define INCLUDE_ODCORE_RENDER_MODEL_H_

#include <vector>

#include <odCore/RefCounted.h>

namespace odRender
{

    class Geometry;

    class Model : public od::RefCounted
    {
    public:

        virtual size_t getGeometryCount() = 0;
        virtual Geometry *getGeometry(size_t index) = 0;
        virtual void addGeometry(Geometry *g) = 0;
        virtual void removeGeometry(Geometry *g) = 0;

        virtual bool geometriesShareVertexData() = 0;

        virtual Geometry *createNewGeometry() = 0;

    };

}


#endif /* INCLUDE_ODCORE_RENDER_MODEL_H_ */
