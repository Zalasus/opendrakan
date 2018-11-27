/*
 * BoundingSphere.h
 *
 *  Created on: 4 Nov 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_BOUNDINGSPHERE_H_
#define INCLUDE_ODCORE_BOUNDINGSPHERE_H_

#include <glm/vec3.hpp>

namespace od
{

    class BoundingSphere
    {
    public:

        BoundingSphere();
        BoundingSphere(const glm::vec3 &center, glm::vec3::value_type radius);
        BoundingSphere(const BoundingSphere &bs);

        BoundingSphere &operator=(const BoundingSphere &bs);

        inline const glm::vec3 &center() const { return mCenter; }
        inline glm::vec3::value_type radius() const { return mRadius; }

        bool intersects(const BoundingSphere &bs, float epsilon = 0.0f);


    private:

        glm::vec3 mCenter;
        glm::vec3::value_type mRadius;

    };

}

#endif /* INCLUDE_ODCORE_BOUNDINGSPHERE_H_ */
