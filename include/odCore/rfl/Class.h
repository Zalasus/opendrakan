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
#include <odCore/Exception.h>
#include <odCore/Message.h>
#include <odCore/Downcast.h>

#include <odCore/rfl/FieldProbe.h>

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
     * @brief Interface for a class implementation.
     *
     * The subclasses ClassImplServer and ClassImplClient extend this interface with client/server specific functions.
     */
    class ClassBase
    {
    public:

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
    };


    template <typename T>
    class ClassImpl : public ClassBase
    {
    public:

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
     * This provides the implementer with a od::Client object, which can be used to access
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
     * This provides the implementer with a od::Server object, which can be used to access
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
         * @brief Allocates a new class instace. Will never return nullptr.
         */
        virtual std::unique_ptr<ClassBase> makeInstance(od::Engine &engine) = 0;

    };

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

        using Fields = _Fields;
        using ClientClassImpl = _ClientClassImpl;
        using ServerClassImpl = _ServerClassImpl;

        static_assert(std::is_base_of<odRfl::FieldBundle, _Fields>::value, "_Fields must implement odRfl::FieldBundle");
        static_assert(!std::is_abstract<_Fields>::value, "_Fields must not be abstract");

        static_assert(std::is_base_of<odRfl::ClientClass, _ClientClassImpl>::value, "_ClientClassImpl must implement odRfl::ClientClass");
        static_assert(!std::is_abstract<_ClientClassImpl>::value, "_ClientClassImpl must not be abstract");

        static_assert(std::is_base_of<odRfl::ServerClass, _ServerClassImpl>::value, "_ServerClassImpl must implement odRfl::ServerClass");
        static_assert(!std::is_abstract<_ServerClassImpl>::value, "_ServerClassImpl must not be abstract");

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
                return clientImpl;

            }else if(engine.isServer())
            {
                auto serverImpl = std::make_unique<_ServerClassImpl>();
                serverImpl->setServer(engine.getServer());
                return serverImpl;

            }else
            {
                throw od::Exception("Failed to instantiate class due to invalid Engine variant state");
            }
        }
    };


    /**
     * @brief A class factory that can only instantiate a field bundle.
     *
     * Trying to make a class instance from this will cause a throw.
     */
    template <typename _Fields>
    class FieldsOnlyClassFactory final : public ClassFactory
    {
    public:

        using Fields = _Fields;

        virtual std::unique_ptr<FieldBundle> makeFieldBundle() override
        {
            return std::make_unique<_Fields>();
        }

        virtual std::unique_ptr<ClassBase> makeInstance(od::Engine &engine) override
        {
            throw od::Exception("Tried to make instance of field-only class");
        }

    };


    template <ClassId _Id, const char *_Category, const char *_Name, typename _Factory>
    class ClassDefinition
    {
    public:

        static_assert(std::is_base_of<odRfl::ClassFactory, _Factory>::value, "_Factory must implement odRfl::ClassFactory");
        static_assert(!std::is_abstract<_Factory>::value, "_Factory must not be abstract");

        using Factory = _Factory;

        static inline constexpr ClassId classId() { return _Id; }
        static inline constexpr const char *category() { return _Category; }
        static inline constexpr const char *name() { return _Name; }

        /**
         * @brief Returns a factory for this class with static storage duration.
         */
        static ClassFactory &getFactory()
        {
            static _Factory factory;
            return factory;
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
