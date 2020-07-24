
#include <odCore/state/ObjectTransform.h>

#include <odCore/LevelObject.h>

namespace odState
{

    ObjectTransform::ObjectTransform()
    : mTypeMask(0)
    , mPosition(0, 0, 0)
    , mRotation(0, 0, 0, 1)
    , mScale(1, 1, 1)
    {
    }

    void ObjectTransform::setPosition(const glm::vec3 &p)
    {
        mPosition = p;
        mTypeMask |= TRANSLATED;
    }

    void ObjectTransform::addPosition(const glm::vec3 &p)
    {
        mPosition += p;
        mTypeMask |= TRANSLATED;
    }

    void ObjectTransform::setRotation(const glm::quat &r)
    {
        mRotation = r;
        mTypeMask |= ROTATED;
    }

    void ObjectTransform::addRotation(const glm::quat &r)
    {
        mRotation *= r;
        mTypeMask |= ROTATED;
    }

    void ObjectTransform::setScale(const glm::vec3 &s)
    {
        mScale = s;
        mTypeMask |= SCALED;
    }

    void ObjectTransform::addScale(const glm::vec3 &s)
    {
        mScale *= s;
        mTypeMask |= SCALED;
    }

    ObjectTransform ObjectTransform::merge(const ObjectTransform &tf) const
    {
        ObjectTransform mergedTf(*this);

        if(tf.isTranslation())
        {
            mergedTf.addPosition(tf.getPosition());
        }

        if(tf.isRotation())
        {
            mergedTf.addRotation(tf.getRotation());
        }

        if(tf.isScaling())
        {
            mergedTf.addScale(tf.getScale());
        }

        return mergedTf;
    }

    ObjectTransform ObjectTransform::invert() const
    {
        ObjectTransform tf;

        if(isTranslation())
        {
            tf.setPosition(-getPosition());
        }

        if(isRotation())
        {
            glm::quat inv = glm::inverse(getRotation());  // TODO: is this correct?
            tf.setRotation(inv);
        }

        if(isScaling())
        {
            glm::vec3 inverseScale(1.0/getScale().x, 1.0/getScale().y, 1.0/getScale().z);
            tf.setScale(inverseScale);
        }

        return tf;
    }

    void ObjectTransform::applyTo(od::LevelObject &obj) const
    {
        if(isTranslation()) obj.setPosition(getPosition());
        if(isRotation()) obj.setRotation(getRotation());
        if(isScaling()) obj.setScale(getScale());
    }

}
