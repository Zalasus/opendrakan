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

    class ClassBase
    {
    public:

        ClassBase();
        virtual ~ClassBase() = default;

        void setRfl(Rfl &rfl);
        Rfl &getRfl();

        virtual void probeFields(FieldProbe &probe) = 0;

        virtual void onLoaded();

        virtual ClassBaseType getBaseType();
        virtual LevelObjectClassBase *asLevelObjectBase();
        virtual MaterialClassBase *asMaterialBase();


    private:

        Rfl *mRfl;

    };


    class LevelObjectClassBase : public ClassBase
    {
    public:

        LevelObjectClassBase();

        void setLevelObject(od::LevelObject &obj);
        od::LevelObject &getLevelObject();

        virtual void onSpawned();
        virtual void onDespawned();
        virtual void onUpdate(float relTime);
        virtual void onMessageReceived(od::LevelObject &sender, od::Message message);
        virtual void onDestroyed();
        virtual void onVisibilityChanged();
        virtual void onLayerChanged(od::Layer *from, od::Layer *to);

        virtual void onTranslated(const glm::vec3 &from, const glm::vec3 &to);
        virtual void onRotated(const glm::quat &from, const glm::quat &to);
        virtual void onScaled(const glm::vec3 &from, const glm::vec3 &to);
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
