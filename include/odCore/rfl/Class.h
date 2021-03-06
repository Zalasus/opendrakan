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

#include <odCore/Engine.h>
#include <odCore/Message.h>
#include <odCore/Downcast.h>
#include <odCore/Panic.h>

#include <odCore/rfl/FieldProbe.h>

namespace od
{
    class Client;
    class Server;
    class LevelObject;
    class Layer;
}

namespace odPhysics
{
    class Handle;
}

namespace odState
{
    struct StateBundleBase;
}

namespace odRfl
{
    class Rfl;

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

        virtual ~FieldBundle(); // = default

        //virtual std::unique_ptr<FieldBundle> clone() = 0;

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

    class SpawnableClass;
    class ClientClass;
    class ServerClass;

    /**
     * @brief Interface for all class implementations.
     *
     * This defines the basic set of hooks that every class possesses. More
     * specific interfaces exist for classes that need a server, client or level
     * object.
     *
     * The interface also defines methods for quickly converting an instance of
     * ClassBase into one of the specific server-, client- or spawnable
     * interface without a dynamic cast. An implementation for these methods can
     * be auto-generated using the CRTP ClassImpl<T>. In order to avoid having
     * to manually override the conversion methods inherit from
     * `ClassImpl<YourClass>` instead of ClassBase.
     */
    class ClassBase
    {
    public:

        ClassBase();
        virtual ~ClassBase(); // = default;

        /**
         * @brief Returns a reference to the field bundle used by this implementation.
         */
        virtual FieldBundle &getFields() = 0;

        /**
         * @brief Hook that is called after all fields have been filled.
         */
        virtual void onLoaded();

        virtual SpawnableClass *asSpawnableClass() = 0;
        virtual ClientClass *asClientClass() = 0;
        virtual ServerClass *asServerClass() = 0;

        template <typename T>
        T &getRfl()
        {
            static_assert(std::is_base_of<Rfl, T>::value, "T in getRfl<T>() must have a visible odRfl::Rfl base");

            if(mRfl == nullptr)
            {
                OD_PANIC() << "No RFL set";
            }

            return *od::confident_downcast<T>(mRfl);
        }

        void setRfl(Rfl &rfl);


    private:

        Rfl *mRfl;

    };


    /**
     * @brief CRTP that auto-generates implementations for the conversion methods in ClassBase.
     *
     * In order to avoid having to manually override the conversion methods in
     * ClassBase, inherit from `ClassImpl<YourClass>` instead of ClassBase.
     */
    template <typename T>
    class ClassImpl : public ClassBase
    {
    public:

        // TODO: maybe add field bundle as a protected member here and let this also implement getFields()? same for states

        virtual SpawnableClass *asSpawnableClass() override final
        {
            return od::static_downcast<T, SpawnableClass>(static_cast<T*>(this));
        }

        virtual ClientClass *asClientClass() override final
        {
            return od::static_downcast<T, ClientClass>(static_cast<T*>(this));
        }

        virtual ServerClass *asServerClass() override final
        {
            return od::static_downcast<T, ServerClass>(static_cast<T*>(this));
        }

    };


    /**
     * @brief Interface for a client-side class implementation.
     *
     * This provides the implementer with an od::Client object, which can be used to access
     * the client's subsystems.
     */
    class ClientClass
    {
    public:

        inline void setClient(od::Client &client) { mClient = &client; }
        inline od::Client &getClient() { return *mClient; }


    private:

        od::Client *mClient;

    };


    /**
     * @brief Interface for a server-side class implementation.
     *
     * This provides the implementer with an od::Server object, which can be used to access
     * the server's subsystems.
     */
    class ServerClass
    {
    public:

        inline void setServer(od::Server &server) { mServer = &server; }
        inline od::Server &getServer() { return *mServer; }


    private:

        od::Server *mServer;
    };


    /**
     * @brief Interface for a class that can be applied to a Level Object.
     */
    class SpawnableClass
    {
    public:

        virtual ~SpawnableClass();

        inline void setLevelObject(od::LevelObject &obj) { mLevelObject = &obj; }
        inline od::LevelObject &getLevelObject() { return *mLevelObject; }

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
         * @brief Called when the object gets "started", signaling it to start executing it's programmed behaviour.
         *
         * This happens once after spawning, and whenever an object is brought back up after being stopped
         * e.g. during a sequence.
         *
         * This is where you'd start listening for actions, for instance.
         *
         * Note that the update hook will not be called while an object is stopped, regardless of whether it was activated or not.
         * It will fire again as soon as the object is restarted.
         */
        virtual void onStart();

        /**
         * @brief Called when the object gets "stopped", signaling it to stop executing it's programmed behaviour.
         *
         * A sequence might stop certain objects, so their programmed behaviour does not interfere with the sequence content.
         *
         * Here, you should disable any action listeners or other custom behaviour which might trigger game mechanics.
         *
         * Note that the update hook will not be called while an object is stopped, regardless of whether it was activated or not.
         * It will fire again as soon as the object is restarted.
         */
        virtual void onStop();

        /**
         * @brief Update hook. Will be called every game tick if object is spawned.
         *
         * There is absolutely no guarantee in what order objects will be updated. If your updates require other objects to have been
         * updated, consider doing them in the onPostUpdate() hook.
         *
         * This hook has to be enabled via LevelObject::setEnableUpdate(true). Note that the update hook will not be called while an
         * object is in the "stopped" state, regardless of whether the hook was activated or not. It will resume firing as soon as the
         * object is restarted.
         *
         * @param relTime  Time since the last tick, in seconds.
         */
        virtual void onUpdate(float relTime);

        /**
         * @brief Post-Update hook. Will be called every game tick, after all objects had their update-hook called.
         *
         * This hook has to be enabled via LevelObject::setEnableUpdate(true). It will not be called when the object is stopped, same as onUpdate().
         *
         * @param relTime  Time since the last tick, in seconds.
         */
        virtual void onPostUpdate(float relTime);

        /**
         * @brief Message hook. Called when this object receives a message.
         *
         * @param sender   The object sending this message.
         * @param message  The message sent.
         */
        virtual void onMessageReceived(od::LevelObject &sender, od::Message message);

        /**
         * @brief Called when the objects visibility state changed.
         */
        virtual void onVisibilityChanged(bool newVisibility);

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
         * Note that the engine will never cause transformation by itself. Transformation is a game mechanics thing and as such
         * should only be triggered by an RFL class. The engine will, however, transform an object if it needs to roll back the
         * level to a previous state and will use this hook for that. You should not assume that sequential calls to this hook
         * represent a sequence in game mechanics.
         *
         * Use this only to make your RFL class reflect this object's state, like moving your physics or render handles. Do **not**
         * trigger game mechanics from here.
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
         * It is not specified whether this is called for each single change or groups of changes.
         *
         * This is called right after the respective specific hook has been called.
         */
        virtual void onTransformChanged();

        /**
         * @brief Returns extra states this object has (nullptr if none, the default).
         */
        virtual odState::StateBundleBase *getExtraStates();

        /**
         * @brief Called when any state in the bundle provided by getExtraStates() has changed.
         *
         * It is not specified whether this is called for each single change or groups of changes.
         *
         * In the future, there might be a method that tells you exactly which state changed.
         */
        virtual void onExtraStatesChanged();


    private:

        od::LevelObject *mLevelObject;

    };


    class ClassFactory
    {
    public:

        virtual ~ClassFactory(); // = default;

        /**
         * @brief Allocates a new field bundle. Will never return nullptr.
         */
        virtual std::unique_ptr<FieldBundle> makeFieldBundle() = 0;

        /**
         * @brief Allocates a new class instace. May return nullptr if no instance can be created for the passed engine.
         */
        virtual std::unique_ptr<ClassBase> makeInstance(od::Engine &engine) = 0;

    };


    namespace detail
    {

        template <typename T>
        struct CheckFieldBundleType
        {
            static_assert(std::is_base_of<odRfl::FieldBundle, T>::value, "Fields class must implement odRfl::FieldBundle");
            static_assert(!std::is_abstract<T>::value, "Fields class must not be abstract");

            using Type = T;
        };

        template <typename T>
        struct CheckServerClassType
        {
            static_assert(std::is_base_of<odRfl::ServerClass, T>::value, "Server class must implement odRfl::ServerClass");
            static_assert(std::is_base_of<odRfl::ClassBase, T>::value, "Server class must implement odRfl::ClassBase (likely via odRfl::ClassImpl)");
            static_assert(!std::is_abstract<T>::value, "Server class must not be abstract");

            using Type = T;
        };

        template <typename T>
        struct CheckClientClassType
        {
            static_assert(std::is_base_of<odRfl::ClientClass, T>::value, "Client class must implement odRfl::ClientClass");
            static_assert(std::is_base_of<odRfl::ClassBase, T>::value, "Client class must implement odRfl::ClassBase (likely via odRfl::ClassImpl)");
            static_assert(!std::is_abstract<T>::value, "Client class must not be abstract");

            using Type = T;
        };

    }


    /**
     * @brief A default class factory for RFL classes with two implementations, one for servers and one for clients.
     *
     * The factory selects which one to instantiate based on the Engine variant passed to the makeInstance method.
     * It also handles assigning the Client/Server refs to the newly created instance.
     */
    template <typename _Fields, typename _ClientClassImpl, typename _ServerClassImpl>
    class DefaultClassFactory final : public ClassFactory
    {
    public:

        using Fields = typename detail::CheckFieldBundleType<_Fields>::Type;
        using ClientClassImpl = typename detail::CheckClientClassType<_ClientClassImpl>::Type;
        using ServerClassImpl = typename detail::CheckServerClassType<_ServerClassImpl>::Type;

        virtual std::unique_ptr<FieldBundle> makeFieldBundle() override
        {
            return std::make_unique<_Fields>();
        }

        virtual std::unique_ptr<ClassBase> makeInstance(od::Engine &engine) override
        {
            if(engine.isClient())
            {
                auto clientImpl = std::make_unique<_ClientClassImpl>();
                clientImpl->setClient(engine.getClient());
                return std::move(clientImpl);

            }else if(engine.isServer())
            {
                auto serverImpl = std::make_unique<_ServerClassImpl>();
                serverImpl->setServer(engine.getServer());
                return std::move(serverImpl);

            }else
            {
                OD_PANIC() << "Failed to instantiate class due to invalid Engine variant state";
            }
        }
    };

    /**
     * @brief A class factory that can only instantiate a field bundle.
     *
     * Trying to make a class instance from this will cause a panic.
     */
    template <typename _Fields>
    class FieldsOnlyClassFactory final : public ClassFactory
    {
    public:

        using Fields = typename detail::CheckFieldBundleType<_Fields>::Type;

        virtual std::unique_ptr<FieldBundle> makeFieldBundle() override
        {
            return std::make_unique<_Fields>();
        }

        virtual std::unique_ptr<ClassBase> makeInstance(od::Engine &engine) override
        {
            OD_PANIC() << "Tried to make instance of field-only class";

            // a (yet unfixed) bug in GCC causes it to output a -Wreturn-type
            //  warning here even though the function never gets past the panic.
            //  this bug only occurs in templates under very specific
            //  circumstances. however, this simple return fixes that.
            //  see related: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=79021
            return nullptr;
        }

    };


    /**
     * @brief A class factory for RFL classes with only a server implementation.
     *
     * On clients, will return nullptr as instance to signal back to the caller
     * that no implementation exists. A level, for instance, could delete
     * objects without a valid class implementation.
     */
    template <typename _Fields, typename _ServerClassImpl>
    class ServerOnlyClassFactory final : public ClassFactory
    {
    public:

        using Fields = typename detail::CheckFieldBundleType<_Fields>::Type;
        using ServerClassImpl = typename detail::CheckServerClassType<_ServerClassImpl>::Type;

        virtual std::unique_ptr<FieldBundle> makeFieldBundle() override
        {
            return std::make_unique<_Fields>();
        }

        virtual std::unique_ptr<ClassBase> makeInstance(od::Engine &engine) override
        {
            if(engine.isServer())
            {
                auto serverImpl = std::make_unique<_ServerClassImpl>();
                serverImpl->setServer(engine.getServer());
                return std::move(serverImpl);

            }else
            {
                return nullptr;
            }
        }
    };


    /**
     * @brief A class factory for RFL classes with only a client implementation.
     *
     * On servers, will return nullptr as instance to signal back to the caller
     * that no implementation exists. A level, for instance, could delete
     * objects without a valid class implementation.
     */
    template <typename _Fields, typename _ClientClassImpl>
    class ClientOnlyClassFactory final : public ClassFactory
    {
    public:

        using Fields = typename detail::CheckFieldBundleType<_Fields>::Type;
        using ClientClassImpl = typename detail::CheckClientClassType<_ClientClassImpl>::Type;

        virtual std::unique_ptr<FieldBundle> makeFieldBundle() override
        {
            return std::make_unique<_Fields>();
        }

        virtual std::unique_ptr<ClassBase> makeInstance(od::Engine &engine) override
        {
            if(engine.isClient())
            {
                auto clientImpl = std::make_unique<_ClientClassImpl>();
                clientImpl->setClient(engine.getClient());
                return std::move(clientImpl);

            }else
            {
                return nullptr;
            }
        }
    };

}

#define OD_DEFINE_CLASS(_cppclass, _id, _category, _name, _factory) \
            struct _cppclass \
            { \
                static_assert(std::is_base_of<odRfl::ClassFactory, _factory>::value, "_factory must implement odRfl::ClassFactory"); \
                static_assert(!std::is_abstract<_factory>::value, "_factory must not be abstract"); \
                using Factory = _factory; \
                static inline constexpr odRfl::ClassId classId() { return (_id); } \
                static inline constexpr const char *category() { return (_category); } \
                static inline constexpr const char *name() { return (_name); } \
                static odRfl::ClassFactory &getFactory() \
                { \
                    static _factory factory; \
                    return factory; \
                } \
            }

#endif /* INCLUDE_ODCORE_RFL_CLASS_H_ */
