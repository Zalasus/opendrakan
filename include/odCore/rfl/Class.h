/*
 * @file Class.h
 * @date 4 Sep 2019
 * @author zal
 */

#ifndef INCLUDE_ODCORE_RFL_CLASS_H_
#define INCLUDE_ODCORE_RFL_CLASS_H_

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <odCore/rfl/FieldProbe.h>

#define ODRFL_DEFINE_CLASS(id, category_str, name_str, identifier) \
    struct identifier {}; \
    template <> constexpr odRfl::ClassId odRfl::ClassTraits<identifier>::classId() { return id; } \
    template <> constexpr const char *odRfl::ClassTraits<identifier>::name() { return name_str; } \
    template <> constexpr const char *odRfl::ClassTraits<identifier>::category() { return category_str; }

namespace od
{
    class LevelObject;
    class Layer;
}

namespace odRfl
{
    class Rfl;


    typedef uint16_t ClassId;


    template <typename T>
    struct ClassTraits
    {
        static constexpr ClassId classId();
        static constexpr const char *name();
        static constexpr const char *category();
    };


    class ClassImpl
    {
    public:

        ClassImpl(Rfl &rfl);
        virtual ~ClassImpl() = default;

        virtual void probeFields(FieldProbe &probe) = 0;

        virtual void onLoaded();

    };


    class LevelObjectClassImpl : public ClassImpl
    {
    public:

        LevelObjectClassImpl(Rfl &rfl, od::LevelObject &obj);

        inline od::LevelObject &getLevelObject() { return mLevelObject; };

        virtual void onSpawned();
        virtual void onDespawned();
        virtual void onUpdate(float relTime);
        virtual void onMessageReceived(od::LevelObject &sender, Message message);
        virtual void onDestroyed();
        virtual void onVisibilityChanged();
        virtual void onLayerChanged(od::Layer *from, od::Layer *to);

        virtual void onTranslated(const glm::vec3 &from, const glm::vec3 &to);
        virtual void onRotated(const glm::quat &from, const glm::quat &to);
        virtual void onScaled(const glm::vec3 &from, const glm::vec3 &to);
        virtual void onTransformChanged();


    private:

        od::LevelObject &mLevelObject;
    };

}

#endif /* INCLUDE_ODCORE_RFL_CLASS_H_ */
