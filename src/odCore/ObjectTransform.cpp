
#include <odCore/ObjectTransform.h>

namespace od
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

    bool ObjectTransform::operator==(const ObjectTransform &rhs) const
    {
        if((mTypeMask & TRANSLATED) && (mPosition != rhs.mPosition)) return false;
        if((mTypeMask & ROTATED) && (mRotation != rhs.mRotation)) return false;
        if((mTypeMask & SCALED) && (mScale != rhs.mScale)) return false;
        return true;
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

    ObjectTransform ObjectTransform::lerp(const ObjectTransform &rhs, float delta) const
    {
        ObjectTransform tf;

        if(isTranslation())
        {
            if(rhs.isTranslation())
            {
                tf.setPosition(glm::mix(getPosition(), rhs.getPosition(), delta));

            }else
            {
                tf.setPosition(getPosition());
            }
        }

        if(isRotation())
        {
            if(rhs.isRotation())
            {
                tf.setRotation(glm::slerp(getRotation(), rhs.getRotation(), delta));

            }else
            {
                tf.setRotation(getRotation());
            }
        }

        if(isScaling())
        {
            if(rhs.isScaling())
            {
                tf.setScale(glm::mix(getScale(), rhs.getScale(), delta));

            }else
            {
                tf.setScale(getScale());
            }
        }

        return tf;
    }

}
