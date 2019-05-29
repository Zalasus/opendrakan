/*
 * BoundingBox.h
 *
 *  Created on: 4 Nov 2018
 *      Author: zal
 */

#ifndef INCLUDE_BOUNDINGBOX_H_
#define INCLUDE_BOUNDINGBOX_H_

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

namespace od
{

    class AxisAlignedBoundingBox
    {
    public:

        AxisAlignedBoundingBox();
        AxisAlignedBoundingBox(const glm::vec3 &min, const glm::vec3 &max);
        AxisAlignedBoundingBox(const AxisAlignedBoundingBox &aabb);

        inline glm::vec3 min() const { return mMin; }
        inline glm::vec3 max() const { return mMax; }

        AxisAlignedBoundingBox &operator=(const AxisAlignedBoundingBox &aabb);

        bool intersects(const AxisAlignedBoundingBox &aabb) const;
        bool intersects(const AxisAlignedBoundingBox &aabb, glm::vec3::value_type epsilon) const;

        bool contains(const glm::vec3 &v) const;

        void expandBy(const glm::vec3 &v);


    private:

        glm::vec3 mMin;
        glm::vec3 mMax;
    };

    class OrientedBoundingBox
    {
    public:

        OrientedBoundingBox();
        OrientedBoundingBox(const glm::vec3 &center, const glm::vec3 &extends, const glm::quat &orientation);
        OrientedBoundingBox(const OrientedBoundingBox &obb);

        inline glm::vec3 &extends() { return mExtends; }
        inline glm::vec3 &center() { return mCenter; }
        inline glm::quat &orientation() { return mOrientation; }
        inline const glm::vec3 &extends() const { return mExtends; }
        inline const glm::vec3 &center() const { return mCenter; }
        inline const glm::quat &orientation() const { return mOrientation; }

        OrientedBoundingBox &operator=(const OrientedBoundingBox &obb);


    private:

        glm::vec3 mExtends;
        glm::vec3 mCenter;
        glm::quat mOrientation;

    };

}

#endif /* INCLUDE_BOUNDINGBOX_H_ */
