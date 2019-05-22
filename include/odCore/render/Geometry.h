/*
 * Geometry.h
 *
 *  Created on: Nov 12, 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_GEOMETRY_H_
#define INCLUDE_ODCORE_RENDER_GEOMETRY_H_

#include <vector>
#include <memory>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <odCore/RefCounted.h>

#include <odCore/render/Array.h>

namespace odRender
{

    class Texture;

    enum class PrimitiveType
    {
        LINES,
        TRIANGLES,
        TRIANGLE_FAN,
        TRIANGLE_STRIP
    };

    /**
     * An interface for a renderable geometry, to be implemented by the renderer module.
     * This provides read-write access to the underlying vertex attributes. Note that
     * this data might not be the same as the data found in odDb::Model objects, as that
     * data is optimized for storage and usually not directly renderable.
     *
     * Usually a model needs to be split into multiple geometries, depending on how the renderer
     * implements rendering of multiple textures. This splitting is to to be done by the renderer.
     * In a split model, the vertex attributes can be shared among multiple geometries, so modifying
     * an array in one geometry might change it in all of the model's geometries.
     */
    class Geometry : public od::RefCounted
    {
    public:

        virtual void setHasBoneInfo(bool b) = 0;
        virtual bool hasBoneInfo() const = 0;
        virtual std::unique_ptr<ArrayAccessHandler<glm::vec4>> getBoneIndexArrayAccessHandler() = 0;
        virtual std::unique_ptr<ArrayAccessHandler<glm::vec4>> getBoneWeightArrayAccessHandler() = 0;

        virtual std::unique_ptr<ArrayAccessHandler<glm::vec3>> getVertexArrayAccessHandler() = 0;
        virtual std::unique_ptr<ArrayAccessHandler<glm::vec4>> getColorArrayAccessHandler() = 0;
        virtual std::unique_ptr<ArrayAccessHandler<glm::vec3>> getNormalArrayAccessHandler() = 0;
        virtual std::unique_ptr<ArrayAccessHandler<glm::vec2>> getTextureCoordArrayAccessHandler() = 0;

        virtual std::unique_ptr<ArrayAccessHandler<int32_t>> getIndexArrayAccessHandler() = 0;

        virtual void setTexture(Texture *texture) = 0;

        virtual bool isIndexed() = 0;
        virtual PrimitiveType getPrimitiveType() = 0;

    };

}


#endif /* INCLUDE_ODCORE_RENDER_GEOMETRY_H_ */
