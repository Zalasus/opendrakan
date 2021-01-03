/*
 * Model.h
 *
 *  Created on: May 15, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_MODEL_H_
#define INCLUDE_ODCORE_RENDER_MODEL_H_

#include <vector>
#include <memory>

namespace odRender
{

    class Geometry;

    enum class LightingMode
    {
        OFF,
        AMBIENT_DIFFUSE,
        AMBIENT_DIFFUSE_SPECULAR
    };

    class Model
    {
    public:

        virtual size_t getGeometryCount() = 0;
        virtual std::shared_ptr<Geometry> getGeometry(size_t index) = 0;
        virtual void addGeometry(std::shared_ptr<Geometry> g) = 0;
        virtual void removeGeometry(std::shared_ptr<Geometry> g) = 0;

        virtual bool hasSharedVertexArrays() = 0;

        virtual void setLightingMode(LightingMode lm) = 0;

    };

}


#endif /* INCLUDE_ODCORE_RENDER_MODEL_H_ */
