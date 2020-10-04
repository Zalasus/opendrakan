
#ifndef INCLUDE_ODCORE_STATE_STATE_H_
#define INCLUDE_ODCORE_STATE_STATE_H_

namespace odState
{

    enum class StateType
    {
        INTEGER,
        FLOAT,
        VECTOR_2,
        VECTOR_3,
        VECTOR_4,
        QUATERNION,
        CUSTOM
    };


    struct StateValueVariant
    {
    public:

        StateType getType() const { return mType; }

        template <typename T>
        void visit(T &f)
        {
            switch(mType)
            {
            case INTEGER:
                f(mIntValue);
                break;

            case FLOAT:
                f(mIntValue);
                break;

            case VECTOR_2:
                f(mIntValue);
                break;

            case VECTOR_3:
                f(mIntValue);
                break;

            case VECTOR_4:
                f(mIntValue);
                break;

            case QUATERNION:
                f(mIntValue);
                break;

            case CUSTOM:
                OD_UNREACHABLE(); // can't store custom states in this variant. the constructor should not allow this
            }
        }

    private:

        StateType mType;
        union
        {
            int32_t   mIntValue;
            float     mFloatValue;
            glm::vec2 mVec2Value;
            glm::vec3 mVec3Value;
            glm::vec4 mVec4Value;
            glm::quat mQuatValue;
        };
    };

}

#endif
