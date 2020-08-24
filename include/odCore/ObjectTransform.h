/*
 * @file ObjectTransform.h
 * @author zal
 */

#ifndef INCLUDE_ODCORE_OBJECTTRANSFORM_H_
#define INCLUDE_ODCORE_OBJECTTRANSFORM_H_

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace od
{
    /**
     * @brief Class for encapsulating any combination of tranlation, rotation and scaling.
     *
     * This serves as a way to abstract these three types of transform into a more
     * general "transformed" event which we can pass over the network, while only
     * having to transmit and perform changes that actually happened (as opposed to
     * sending a zero-vector for translation if no translation happened).
     *
     * For now, these operations are absolute only. Might add a "relative"-flag later.
     *
     * While the Riot engine seems to have represented the spatial state of objects as
     * a transform matrix, this seems unnecessarily complex, especially when being sent
     * over a network.
     */
    class ObjectTransform
    {
    public:

        ObjectTransform();
        ObjectTransform(const ObjectTransform &tf) = default;

        inline bool isTranslation() const { return mTypeMask & TRANSLATED; }
        inline bool isRotation() const { return mTypeMask & ROTATED; }
        inline bool isScaling() const { return mTypeMask & SCALED; }

        inline const glm::vec3 &getPosition() const { return mPosition; }
        inline const glm::quat &getRotation() const { return mRotation; }
        inline const glm::vec3 &getScale() const { return mScale; }

        void setPosition(const glm::vec3 &p);
        void addPosition(const glm::vec3 &p);

        void setRotation(const glm::quat &r);
        void addRotation(const glm::quat &r);

        void setScale(const glm::vec3 &s);
        void addScale(const glm::vec3 &s);

        ObjectTransform merge(const ObjectTransform &tf) const;
        ObjectTransform invert() const;


    private:

        using TransformTypeMask = uint8_t;
        static constexpr TransformTypeMask TRANSLATED = 1;
        static constexpr TransformTypeMask ROTATED    = 2;
        static constexpr TransformTypeMask SCALED     = 4;

        TransformTypeMask mTypeMask;
        glm::vec3 mPosition;
        glm::quat mRotation;
        glm::vec3 mScale;

    };

}

#endif
