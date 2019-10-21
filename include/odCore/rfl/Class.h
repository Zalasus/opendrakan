/*
 * @file Class.h
 * @date 4 Sep 2019
 * @author zal
 */

#ifndef INCLUDE_ODCORE_RFL_CLASS_H_
#define INCLUDE_ODCORE_RFL_CLASS_H_

#include <type_traits>
#include <memory>

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <odCore/Exception.h>
#include <odCore/Message.h>
#include <odCore/Downcast.h>

#include <odCore/rfl/FieldProbe.h>

#define ODRFL_DEFINE_CLASS(base_ident, id, category_str, name_str) \
        static_assert(std::is_default_constructible<base_ident>::value, "RFL class base must be default-constructible"); \
        static_assert(std::is_base_of<odRfl::ClassBase, base_ident>::value, "RFL class base must implement odRfl::ClassBase"); \
        namespace odRfl \
        { \
            template <> constexpr ClassId ClassTraits<base_ident>::classId() { return id; } \
            template <> constexpr const char *ClassTraits<base_ident>::name() { return name_str; } \
            template <> constexpr const char *ClassTraits<base_ident>::category() { return category_str; } \
        }

#define ODRFL_DEFINE_CLASS_FIELDS(base_ident, fieldBundle_ident) \
        static_assert(std::is_base_of<odRfl::FieldBundle, fieldBundle_ident>::value, "Field bundle must implement odRfl::FieldBundle"); \
        static_assert(std::is_default_constructible<fieldBundle_ident>::value, "Field bundle must be default-constructible"); \
        static_assert(!std::is_abstract<fieldBundle_ident>::value, "Field bundle must not be abstract"); \
        namespace odRfl \
        { \
            template <> \
            struct ClassFieldTraits<base_ident> \
            { \
                using type = fieldBundle_ident; \
            }; \
        }

#define ODRFL_DEFINE_CUSTOM_CLASSFACTORY(base_ident, factory_ident) \
        static_assert(std::is_default_constructible<factory_ident>::value, "Custom RFL class factory must be default-constructible"); \
        static_assert(std::is_base_of<odRfl::ClassFactory, factory_ident>::value, "Custom RFL class factory must implement odRfl::ClassFactory"); \
        namespace odRfl \
        { \
            template <> ClassFactory &ClassTraits<base_ident>::getFactory() { static factory_ident factory; return factory; } \
        }

#define ODRFL_DEFINE_CLASS_IMPL_CLIENT(base_ident, impl_ident) \
        static_assert(std::is_base_of<odRfl::ClassBase, base_ident>::value, "Base of RFL class implementation must implement odRfl::ClassBase"); \
        static_assert(std::is_base_of<odRfl::ClassImpl, impl_ident>::value, "RFL client class implementation must implement odRfl::ClassImpl"); \
        static_assert(std::is_base_of<base_ident, impl_ident>::value, "RFL class implementation must inherit from the given base "); \
        namespace odRfl \
        { \
            template <> \
            struct ClassImplTraits<base_ident, odRfl::ImplType::SERVER> \
            { \
                using type = impl_ident; \
            }; \
        }

#define ODRFL_DEFINE_CLASS_IMPL_SERVER(base_ident, impl_ident) \
        static_assert(std::is_base_of<odRfl::ClassBase, base_ident>::value, "Base of RFL class implementation must implement odRfl::ClassBase"); \
        static_assert(std::is_base_of<odRfl::ClassImpl, impl_ident>::value, "RFL server class implementation must implement odRfl::ClassImpl"); \
        static_assert(std::is_base_of<base_ident, impl_ident>::value, "RFL class implementation must inherit from the given base "); \
        namespace odRfl \
        { \
            template <> \
            struct ClassImplTraits<base_ident, odRfl::ImplType::CLIENT> \
            { \
                using type = impl_ident; \
            }; \
        }

namespace od
{
    class Client;
    class Server;
    class LevelObject;
    class Layer;
}

namespace odRfl
{

    typedef uint16_t ClassId;


    /**
     * @brief Interface for containers of RFL class fields. This allows introspection of your RFL class's configurable fields at runtime.
     *
     * The basic idea is this: You create a class implementing this interface. Your class contains the configurable
     * fields as data members. To make those fields visible to the engine, you register them with an odRfl::FieldProbe
     * in the probeFields() method. Default values of your fields are (obviously) assigned in the default constructor.
     *
     * Your FieldBundle subclass can be instantiated standalone (for instance if the editor wants to know the fields that are
     * available in your class in order to list them a dialog) OR it can be instantiated as part of the actual RFL class
     * implementation that provides class behaviour and thus needs access to the field values the level designer has selected.
     * In the latter case, the registration of the fields would also cause them to be filled with the values from the level files.
     *
     * TODO: recommend inheritance or composition for using field bundle in RFL class implementation?
     * the latter would either mean we'd have to use public data members, or define an unreasonable amount of getters for some classes.
     */
    class FieldBundle
    {
    public:

        virtual ~FieldBundle() = default;

        /**
         * @brief Method for probing the configurable class fields.
         *
         * Register all fields that need to be exposed to the engine here. Probing allows the engine to list the fields a
         * class has or to fill them with values.
         *
         * FieldProbe is a visitor that can implement any operation on class fields. Among them are listing fields, filling fields
         * or prefetching all asset references. This is completely up to the FieldProbe. You only have to register your fields here.
         *
         * This must be able to be called any number of times. The order/type/number etc. of fields probed here must not change
         * from one probing to another.
         *
         * Do not do anything other than registering your fields with the probe in this method!
         *
         * For performance and safety reasons, you should probably declare your override of this as final.
         */
        virtual void probeFields(FieldProbe &probe) = 0;

    };

    /**
     * @brief Traits class linking a class base with a field bundle.
     *
     * If _Base is a valid class base, this will contain a typedef named \c type aliasing the
     * class' FieldBundle implementation.
     *
     * There is no need to specialize this yourself. Use the convenience macro ODRFL_DEFINE_CLASS_FIELDS() instead.
     */
    template <typename _Base>
    struct ClassFieldTraits
    {
    };


    class Spawnable;

    /**
     * @brief Interface for all RFL class bases.
     *
     * Your implementation of this is what defines your RFL class. It is what you pass to the ODRFL_DEFINE_CLASS() macro in order
     * to link an RFL class ID and name to your C++ class (this happens via the ClassTraits<...> template).
     *
     * It is where you put code that is shared among all implementations of your class.
     *
     * This interface provides methods to convert your base to an interface for different engine objects. For instance, if your
     * class is supposed to be placed on a level object, it would implement the Spawnable interface, and override asSpawnable()
     * to return itself as a Spawnable. This way, costly dynamic_casts are avoided.
     *
     * You probably don't want to implement this directly, but use ClassBaseWrapper<...> with your implementation
     * as template argument instead. The CRTP ClassBaseWrapper checks properties your ClassBase should have via static_asserts and
     * provides implementations for the fast-casting methods (asSpawnable() etc.), since it knows which interfaces your base implements.
     * FIXME: that does not work yet. you have to provide the overrides yourself
     *
     */
    class ClassBase
    {
    public:

        virtual ~ClassBase() = default;

        virtual Spawnable *asSpawnable();

    };


    /**
     * @brief Convenience template implementing common methods
     *
     * FIXME: great idea, but the static_cast does not work that way. it has no way of knowing that there is in fact a _DerivedBase
     * subclassing the CRTP wrapper.
     */
    template <typename _DerivedBase, typename _FieldBundle>
    class ClassBaseWrapper : public ClassBase, public _FieldBundle
    {
        static_assert(std::is_default_constructible<_FieldBundle>::value, "Field bundle must be default-constructible");
        static_assert(!std::is_abstract<_FieldBundle>::value, "Field bundle must not be abstract");

    public:

        virtual Spawnable *asSpawnable() override
        {
            if(std::is_base_of<Spawnable, _DerivedBase>::value)
            {
                return static_cast<Spawnable*>(this);

            }else
            {
                return nullptr;
            }
        }

    };


    /**
     * @brief Interface for a class implementation.
     */
    class ClassImpl
    {
    public:

        virtual ~ClassImpl() = default;

        /**
         * @brief Hook that is called after all fields have been filled.
         *
         * This purely means that the fields have been filled and the RFL instance is available via getRfl(). No other
         * assumptions can be made about an instance's state when this is called.
         *
         * For LevelObjectClassBase, there will also be a level object available at this point. This is the point where you
         * would set the object's SpawnStrategy.
         */
        virtual void onLoaded();

    };


    class ClientClassImpl  : public ClassImpl
    {
    public:

        ClientClassImpl(od::Client &client);

        inline od::Client &getClient() { return mClient; }


    private:

        od::Client &mClient;

    };


    class ServerClassImpl : public ClassImpl
    {
    public:

        ServerClassImpl(od::Server &server);

        inline od::Server &getServer() { return mServer; }


    private:

        od::Server &mServer;
    };


    class Spawnable
    {
    public:

        virtual ~Spawnable();

        od::LevelObject &getLevelObject();

        /**
         * @brief Spawned hook. Part of the object lifecycle. Called when an object becomes active in the level.
         *
         * How and when this happens depends on the SpawnStrategy set on the object.
         */
        virtual void onSpawned();

        /**
         * @brief Despawn hook. Part of the object lifecycle. Called when an object is no longer active in the level.
         *
         * Will be called once an object no longer satisfies it's SpawnStrategy. Any non-essential resources held should be
         * released here.
         *
         * This will also be called right before an object is destroyed.
         */
        virtual void onDespawned();

        /**
         * @brief Update hook. Will be called every game tick if object is spawned.
         *
         * There is absolutely no guarantee in what order objects will be updated. If your updates require other objects to have been
         * updated, consider doing them in the onPostUpdate() hook (TBA).
         *
         * Right now, this hook has to be enabled via LevelObject::setEnableRflUpdateHook(true). That might change in the future,
         * since that feature is not really useful and probably does not make that much of a difference performance-wise.
         *
         * @param relTime  Time since the last tick, in seconds.
         */
        virtual void onUpdate(float relTime);

        /**
         * @brief Message hook. Called when this object receives a message.
         *
         * @param sender   The object sending this message.
         * @param message  The message sent.
         */
        virtual void onMessageReceived(od::LevelObject &sender, od::Message message);

        /**
         * @brief Called when this object was destroyed.
         *
         * After this call returns, the RFL class instance might get deallocated. No further hooks will be called.
         * The LevelObject will stay valid even after this returns, however.
         */
        virtual void onDestroyed();

        /**
         * @brief Called when the objects visibility state changed.
         */
        virtual void onVisibilityChanged();

        /**
         * @brief Called when the object's layer association changed. This only happens as a result of object translation.
         *
         * It will not be called when the object is spawned and the initial layer association is determined.
         *
         * @param from  The layer the object was previously associated with. May be nullptr if none.
         * @param to    The layer the object has been moved to. May be nullptr if none.
         */
        virtual void onLayerChanged(od::Layer *from, od::Layer *to);

        /**
         * @brief Called when the object has changed it's position.
         *
         * @param from  The object's previous position
         * @param to    The object's new position
         */
        virtual void onTranslated(const glm::vec3 &from, const glm::vec3 &to);

        /**
         * @brief Called when the object has changed it's orientation.
         *
         * @param from  The object's previous orientation
         * @param to    The object's new orientation
         */
        virtual void onRotated(const glm::quat &from, const glm::quat &to);

        /**
         * @brief Called when the object has changed it's scale.
         *
         * @param from  The object's previous scale
         * @param to    The object's new scale
         */
        virtual void onScaled(const glm::vec3 &from, const glm::vec3 &to);

        /**
         * @brief Called when any transform parameter has changed (translation, rotation, scale).
         *
         * This is called right after the respective specific hook has been called.
         */
        virtual void onTransformChanged();
    };


    enum class ImplType
    {
        SERVER,
        CLIENT
    };

    template <typename _Base, ImplType _Type>
    struct ClassImplTraits
    {
    };


    class ClassFactory
    {
    public:

        virtual ~ClassFactory() = default;

        virtual std::unique_ptr<FieldBundle> makeFieldBundle() = 0;
        virtual std::unique_ptr<ClassBase> makeClientInstance(od::Client &client) = 0;
        virtual std::unique_ptr<ClassBase> makeServerInstance(od::Server &server) = 0;

    };

    template <typename T>
    class DefaultClassFactory : public ClassFactory
    {
    public:

        virtual std::unique_ptr<FieldBundle> makeFieldBundle() override
        {
            using Bundle = typename ClassFieldTraits<T>::type;
            return std::make_unique<Bundle>();
        }

        virtual std::unique_ptr<ClassBase> makeClientInstance(od::Client &client) override
        {
            using Impl = typename ClassImplTraits<T, ImplType::CLIENT>::type;
            return std::make_unique<Impl>(client);
        }

        virtual std::unique_ptr<ClassBase> makeServerInstance(od::Server &server) override
        {
            using Impl = typename ClassImplTraits<T, ImplType::SERVER>::type;
            return std::make_unique<Impl>(server);
        }

    };


    template <typename _Base>
    struct ClassTraits
    {
        static constexpr ClassId classId();
        static constexpr const char *name();
        static constexpr const char *category();

        static ClassFactory &getFactory()
        {
            static DefaultClassFactory<_Base> factory;

            return factory;
        }
    };

}

#endif /* INCLUDE_ODCORE_RFL_CLASS_H_ */
