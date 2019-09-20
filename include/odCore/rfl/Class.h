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

#define ODRFL_DEFINE_CLASS_BASE(base_identifier, id, category_str, name_str) \
    namespace odRfl \
    { \
        template <> constexpr ClassId ClassTraits<base_identifier>::classId() { return id; } \
        template <> constexpr const char *ClassTraits<base_identifier>::name() { return name_str; } \
        template <> constexpr const char *ClassTraits<base_identifier>::category() { return category_str; } \
    }

namespace od
{
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
     * @brief Base for all RFL classes. Most of the time, you won't be extending this directly, but a derived version instead.
     */
    class ClassBase
    {
    public:

        ClassBase();
        virtual ~ClassBase() = default;

        void setRfl(Rfl &rfl);
        Rfl &getRfl();

        /**
         * @brief Obtains the associated RFL as a subtype.
         *
         * FIXME: Dirty hack. Since we know the RFL type at compile time, isn't there a better way to perform a safe cast?
         */
        template <typename _Rfl>
        _Rfl &getRflAs()
        {
            return *od::confident_downcast<_Rfl>(&getRfl());
        }

        /**
         * @brief Method for probing the configurable class fields.
         *
         * Register all fields that need to be exposed to the engine here. Probing allows the engine to list the fields a
         * class has or to fill them with values.
         *
         * This might be called multiple times during the loading procedure.
         *
         * Do not do anything other than registering your fields with the probe in this method!
         */
        virtual void probeFields(FieldProbe &probe) = 0;

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

        /**
         * @brief Returns this instance's base type.
         */
        virtual ClassBaseType getBaseType();

        /**
         * @brief Efficient downcast. Returns this instance as a LevelObjectClassBase, or nullptr if it is not a LevelObjectClassBase.
         */
        virtual LevelObjectClassBase *asLevelObjectBase();

        /**
         * @brief Efficient downcast. Returns this instance as a MaterialClassBase, or nullptr if it is not a MaterialClassBase.
         */
        virtual MaterialClassBase *asMaterialBase();


    private:

        Rfl *mRfl;

    };


    /**
     * @brief Base class for all classes that can be applied to a level object.
     *
     * This defines most of the hooks that allow an RFL class to react to changes to a level object.
     */
    class LevelObjectClassBase : public ClassBase
    {
    public:

        LevelObjectClassBase();

        void setLevelObject(od::LevelObject &obj);
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

        virtual ClassBaseType getBaseType() override;
        virtual LevelObjectClassBase *asLevelObjectBase() override;


    private:

        od::LevelObject *mLevelObject;
    };


    class MaterialClassBase : public ClassBase
    {
    public:

        virtual ClassBaseType getBaseType() override;
        virtual MaterialClassBase *asMaterialBase() override;

    };


    class ClassRegistrar
    {
    public:

        virtual ~ClassRegistrar() = default;

        virtual std::unique_ptr<ClassBase> makeInstance() = 0;

    };

    template <typename T>
    class ClassRegistrarImpl : public ClassRegistrar
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

        static ClassRegistrar &getRegistrar()
        {
            static ClassRegistrarImpl<_Base> registrar;

            return registrar;
        }
    };

}

#endif /* INCLUDE_ODCORE_RFL_CLASS_H_ */
