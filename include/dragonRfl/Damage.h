
#ifndef INCLUDE_DRAGONRFL_DAMAGE_H_
#define INCLUDE_DRAGONRFL_DAMAGE_H_

namespace od
{
    class LevelObject;
}

namespace dragonRfl
{

    enum class DamageType
    {
        NORMAL,
        FIRE,
        ICE,
        POISON
    };

    struct Damage
    {
        DamageType type;
        float strength;
        bool armorPenetrating;
    };

    enum class DamageResult
    {
        IGNORED,
        DODGED,
        HIT
    };

    class Damageable
    {
    public:

        virtual ~Damageable() = default;

        /**
         * @brief Called when an attacker initiates an attack (i.e. takes a swing) that will hit this Damageable.
         *
         * This can be used to decide whether to dodge, for instance.
         *
         * The default implementation does nothing.
         */
        virtual void onPreAttack(od::LevelObject &attacker);

        /**
         * @brief Called when an attack by attacker hits this Damageable.
         */
        virtual DamageResult onAttackHit(od::LevelObject &attacker, const Damage &damage) = 0;

    };

}

#endif
