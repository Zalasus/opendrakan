
#ifndef INCLUDE_ODCORE_STATE_STATEBUNDLE_H_
#define INCLUDE_ODCORE_STATE_STATEBUNDLE_H_

#include <odCore/Downcast.h>

#include <odCore/state/StateBundleDetail.h>

namespace odState
{

    struct StateBundleBase
    {
        virtual ~StateBundleBase() = default;
        virtual size_t countStatesWithValue() const = 0;
        virtual void merge(const StateBundleBase &lhs, const StateBundleBase &rhs) = 0;
        virtual void lerp(const StateBundleBase &lhs, const StateBundleBase &rhs, float delta) = 0;
        virtual void deltaEncode(const StateBundleBase &reference, const StateBundleBase &toEncode) = 0;
        virtual void serialize(od::DataWriter &writer, StateSerializationPurpose purpose) const = 0;
        virtual void deserialize(od::DataReader &reader, StateSerializationPurpose purpose) = 0;
    };


    template <typename _Bundle>
    class StateBundle : public StateBundleBase
    {
    public:

        virtual size_t countStatesWithValue() const override final
        {
            auto &thisSub = static_cast<const _Bundle&>(*this);
            detail::StateCountOp<_Bundle> op(thisSub);
            _Bundle::stateOp(op);
            return op.getCount();
        }

        void merge(const _Bundle &lhs, const _Bundle &rhs)
        {
            auto &result = static_cast<_Bundle&>(*this);
            detail::StateMergeOp<_Bundle> op(lhs, rhs, result);
            _Bundle::stateOp(op);
        }

        virtual void merge(const StateBundleBase &left, const StateBundleBase &right) override final
        {
            auto &lhs = *(od::downcast<const _Bundle>(&left));
            auto &rhs = *(od::downcast<const _Bundle>(&right));
            merge(lhs, rhs);
        }

        void lerp(const _Bundle &lhs, const _Bundle &rhs, float delta)
        {
            auto &result = static_cast<_Bundle&>(*this);
            detail::StateLerpOp<_Bundle> op(lhs, rhs, result, delta);
            _Bundle::stateOp(op);
        }

        virtual void lerp(const StateBundleBase &left, const StateBundleBase &right, float delta) override final
        {
            auto &lhs = *(od::downcast<const _Bundle>(&left));
            auto &rhs = *(od::downcast<const _Bundle>(&right));
            lerp(lhs, rhs, delta);
        }

        void deltaEncode(const _Bundle &reference, const _Bundle &toEncode)
        {
            auto &result = static_cast<_Bundle&>(*this);
            detail::StateDeltaEncOp<_Bundle> op(reference, toEncode, result);
            _Bundle::stateOp(op);
        }

        virtual void deltaEncode(const StateBundleBase &prev, const StateBundleBase &current) override final
        {
            auto &prevSub = *(od::downcast<const _Bundle>(&prev));
            auto &currentSub = *(od::downcast<const _Bundle>(&current));
            deltaEncode(prevSub, currentSub);
        }

        virtual void serialize(od::DataWriter &writer, StateSerializationPurpose purpose) const override final
        {
            auto &bundle = static_cast<const _Bundle&>(*this);
            detail::StateSerializeOp<_Bundle> op(bundle, writer, purpose);
            _Bundle::stateOp(op);
        }

        virtual void deserialize(od::DataReader &reader, StateSerializationPurpose purpose) override final
        {
            auto &bundle = static_cast<_Bundle&>(*this);
            detail::StateDeserializeOp<_Bundle> op(bundle, reader, purpose);
            _Bundle::stateOp(op);
        }


    protected:

        using BundleType = _Bundle; // this alias is used by the OD_STATE macros


    private:

        friend _Bundle;

        // private constructor, but _Bundle is a friend, so it is the only thing that can construct us
        StateBundle() = default;

    };

    /**
     * @brief Helper macro for generating the stateOp function in StateBundles.
     *
     * This begins the list of states. Do not terminate with a semicolon!
     */
    #define OD_BEGIN_STATE_LIST() template <typename T> static void stateOp(T &op) { op

    /**
     * @brief Helper macro for generating the stateOp function in StateBundles.
     *
     * Adds a state to the state list. Call for every state in your bundle,
     * between a call to OD_BEGIN_STATE_LIST and OD_END_STATE_LIST. Do not put
     * a semicolon after this!
     */
    #define OD_STATE(name, flags) (&BundleType::name, (flags))

    /**
     * @brief Helper macro for generating the stateOp function in StateBundles.
     *
     * Finalizes the state list. Do not put a semicolon after this!
     */
    #define OD_END_STATE_LIST() ; }

}

#endif
