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

#define ODRFL_DEFINE_CUSTOM_CLASSFACTORY(base_ident, factory_ident) \
        static_assert(std::is_default_constructible<factory_ident>::value, "Custom RFL class factory must be default-constructible"); \
        static_assert(std::is_base_of<odRfl::ClassFactory, factory_ident>::value, "Custom RFL class factory must implement odRfl::ClassFactory"); \
        namespace odRfl \
        { \
            template <> ClassFactory &ClassTraits<base_ident>::getFactory() { static factory_ident factory; return factory; } \
        }

#define ODRFL_DEFINE_CLASS_IMPL_CLIENT(base_ident, impl_ident) \
        static_assert(std::is_base_of<odRfl::ClassBase, base_ident>::value, "Base of RFL class implementation must implement odRfl::ClassBase"); \
        static_assert(std::is_base_of<odRfl::ClientClassImpl, impl_ident>::value, "RFL client class implementation must implement odRfl::ClientClassImpl"); \
        static_assert(std::is_base_of<base_ident, impl_ident>::value, "RFL class implementation must inherit from the given base "); \

#define ODRFL_DEFINE_CLASS_IMPL_SERVER(base_ident, impl_ident) \
        static_assert(std::is_base_of<odRfl::ClassBase, base_ident>::value, "Base of RFL class implementation must implement odRfl::ClassBase"); \
        static_assert(std::is_base_of<odRfl::ServerClassImpl, impl_ident>::value, "RFL server class implementation must implement odRfl::ServerClassImpl"); \
        static_assert(std::is_base_of<base_ident, impl_ident>::value, "RFL class implementation must inherit from the given base "); \

namespace od
{
    class Client;
    class Server;
    class LevelObject;
    class Layer;
}

namespace odRfl
{
    class Rfl;


    typedef uint16_t ClassId;


    enum class ClassBaseType
    {
        DEFAULT,
        LEVEL_OBJECT,
        MATERIAL
    };


    class LevelObjectClassBase;
    class MaterialClassBase;

    /**
     * @brief Interface for containers of RFL class fields. This allows introspection of your RFL class's configurable fields at runtime.
     *
     * The basic idea is this: You create a class implementing this interface. Your class contains the configurable
     * fields as data members. To make those fields visible to the engine, you register them with an odRfl::FieldProbe
     * in the probeFields() method. Default values of your fields are (obviously) assigned in the constructor.
     *
     * Your subclass can be instantiated standalone, for instance if the editor wants to know the fields that are
     * available in your class in order to list them a dialog, OR it can be instantiated as part of the actual RFL class
     * implementation that provides class behaviour and thus needs access to the field values the level designer has selected.
     * In the latter case, the registration of the fields would also cause them to be filled with the values from the level files.
     *
     * TODO: recommend inheritance or composition for using field bundle in RFL class implementation?
     * the latter would either mean we'd have to use public data members, or define an unreasonable amount of getters for some classes.
     */
    class FieldProbeable
    {
    public:

        virtual ~FieldProbeable() = default;

        /**
         * @brief Method for probing the configurable class fields.
         *
         * Register all fields that need to be exposed to the engine here. Probing allows the engine to list the fields a
         * class has or to fill them with values.
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
     * @brief Interface for all RFL class bases.
     *
     * Your implementation of this is what defines your RFL class.
     */
    class ClassBase : public FieldProbeable
    {
    public:

        virtual ~ClassBase() = default;

        /**
         * @brief Hook that is called after all fields have been filled.
         *
         * This purely means that the fields have been filled and the RFL instance is available via getRfl(). No other
         * assumptions can be made about an instance's state when this is called.
         *
         * For LevelObjectClassBase, there will also be a level object available at this point. This is the point where you
         * would set the object's SpawnStrategy.
         */
        virtual void onLoaded() = 0;

    };


    class MaterialClassBase : public ClassBase
    {
    public:

        virtual ClassBaseType getBaseType() override;
        virtual MaterialClassBase *asMaterialBase() override;

    };


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


    class ClassFactory
    {
    public:

        virtual ~ClassFactory() = default;

        virtual std::unique_ptr<ClassBase> makeInstance() = 0;

    };

    template <typename T>
    class DefaultClassFactory : public ClassFactory
    {
    public:

        virtual std::unique_ptr<ClassBase> makeInstance() override
        {
            return std::make_unique<T>();
        }

    };


    template <typename _Base>
    struct ClassTraits
    {
        static constexpr ClassId classId();
        static constexpr const char *name();
        static constexpr const char *category();

        static constexpr ClassBaseType baseType()
        {
            if(std::is_base_of<LevelObjectClassBase, _Base>::value)
                return ClassBaseType::LEVEL_OBJECT;
            else if(std::is_base_of<MaterialClassBase, _Base>::value)
                return ClassBaseType::MATERIAL;
            else
                return ClassBaseType::DEFAULT;
        }

        static ClassFactory &getFactory()
        {
            static DefaultClassFactory<_Base> factory;

            return factory;
        }
    };
}

#endif /* INCLUDE_ODCORE_RFL_CLASS_H_ */
