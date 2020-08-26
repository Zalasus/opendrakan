
#ifndef INCLUDE_ODCORE_INPUT_ANALOGACTION_H_
#define INCLUDE_ODCORE_INPUT_ANALOGACTION_H_

#include <vector>
#include <functional>
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

        using CallbackType = std::function<void(_ActionEnum, const glm::vec2 &)>;

        AnalogActionHandle(_ActionEnum action)
        : AnalogActionHandleBase(static_cast<ActionCode>(action))
        {
        }

        inline _ActionEnum getAction() const { return static_cast<_ActionEnum>(mActionCode); }

        void addCallback(const CallbackType &callback)
        {
            mCallbacks.push_back(callback);
        }

        virtual void triggerCallback(const glm::vec2 &p) override
        {
            for(auto &cb : mCallbacks)
            {
                cb(getAction(), p);
            }
        }


    private:

        std::vector<CallbackType> mCallbacks;

    };

}

#endif
