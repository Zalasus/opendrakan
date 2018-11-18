/*
 * Geometry.h
 *
 *  Created on: Nov 12, 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_GEOMETRY_H_
#define INCLUDE_ODCORE_RENDER_GEOMETRY_H_

#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace odDb
{
    class Texture;
}

namespace odRender
{

    class GeometrySegment
    {
    public:

        virtual ~GeometrySegment() = default;

        virtual size_t getNumIndices() = 0;
        virtual void addIndex(int index) = 0;

    };

    /**
     * An interface for a renderable geometry, to be implemented by the renderer module.
     * This provides direct read-write access to the underlying vertex attributes. Note that
     * this data is not the same as the data found in odDb::Model objects, as that data is
     * optimized for storage and usually not directly renderable.
     *
     * If the underlying rendering system can not use the vector types expected to be provided
     * by this interface, it is the implementations job to convert and copy them to it's
     * represantation of choice. This makes it necessary that a user always calls notifyVertexDataChanged()
     * after making edits to the vertex data, so the implementation can update it's local data.
     */
    class Geometry
    {
    public:

        virtual ~Geometry() = default;

        virtual std::vector<glm::vec3> &getVertexArray() = 0;
        virtual std::vector<glm::vec4> &getColorArray() = 0;
        virtual std::vector<glm::vec3> &getNormalArray() = 0;
        virtual std::vector<glm::vec2> &getTextureCoordArray() = 0;

        virtual void setHasBoneInfo(bool b) = 0;
        virtual bool hasBoneInfo() const = 0;
        virtual std::vector<glm::vec4> &getBoneIndexArray() = 0;
        virtual std::vector<glm::vec4> &getBoneWeightArray() = 0;

        /**
         * Notifies the underlying geometry that the vertex data has changes. If \c lastUpdate
         * is true the implementation is allowed to clear the vectors and throw if any more
         * accesses are made to the vertex data. This can be used to save memory for rendering
         * systems that need to make copies of the vertex data in order to use it.
         */
        virtual void notifyVertexDataChanged(bool lastUpdate) = 0;

        virtual GeometrySegment *addSegment(size_t indexCount, odDb::Texture *texture) = 0;

    };

}


#endif /* INCLUDE_ODCORE_RENDER_GEOMETRY_H_ */
