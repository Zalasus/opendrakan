
#ifndef INCLUDE_ODCORE_INPUT_ANALOGACTION_H_
#define INCLUDE_ODCORE_INPUT_ANALOGACTION_H_

#include <glm/vec2.hpp>

namespace odInput
{

    enum class AnalogSource
    {
        MOUSE_POSITION,
        MOUSE_WHEEL
    };

    /**
     * In contrast to an ActionBase, which is digital and can only be on or off,
     * this represents an action that can have a float value on one or two axes.
     *
     * This can be used to access the mouse wheel
     */
    class AnalogActionHandleBase
    {
    public:

        AnalogActionHandleBase(ActionCode code)
        : mActionCode(code)
        {
        }

        virtual ~AnalogActionHandleBase() = default;

        inline void setFlipYAxis(bool b) { mFlipYAxis = b; }
        inline bool flipYAxis() const { return mFlipYAxis; }
        inline ActionCode getActionCode() const { return mActionCode; }

        /**
         * For sources with only one axis, the position is represented by either
         * x or y. Both values will be equal.
         */
        virtual void triggerCallback(const glm::vec2 &p) = 0;


    protected:

        ActionCode mActionCode;

    private:

        bool mFlipYAxis;

    };

    template <typename _ActionEnum>
    class AnalogActionHandle final : public AnalogActionHandleBase
    {
    public:

        AnalogActionHandle(_ActionEnum action)
        : AnalogActionHandleBase(static_cast<ActionCode>(action))
        {
        }

        inline _ActionEnum getAction() const { return static_cast<_ActionEnum>(mActionCode); }

        virtual void triggerCallback(const glm::vec2 &p) override
        {
        }

    };

}

#endif
