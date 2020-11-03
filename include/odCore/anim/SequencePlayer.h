
#ifndef INCLUDE_ODCORE_ANIM_SEQUENCEPLAYER_H_
#define INCLUDE_ODCORE_ANIM_SEQUENCEPLAYER_H_

#include <vector>
#include <unordered_map>

#include <odCore/IdTypes.h>

#include <odCore/db/Sequence.h>

namespace od
{
    class Level;
    class LevelObject;
}

namespace odDb
{
    class Animation;
    class Sound;
}

namespace odAnim
{
    struct PlayerActionStartAnim : odDb::ActionStartAnim
    {
        PlayerActionStartAnim(const odDb::ActionStartAnim &a)
        : odDb::ActionStartAnim(a)
        {
        }

        std::shared_ptr<odDb::Animation> animation;
    };

    struct PlayerActionPlaySound : odDb::ActionPlaySound
    {
        PlayerActionPlaySound(const odDb::ActionPlaySound &a)
        : odDb::ActionPlaySound(a)
        {
        }

        std::shared_ptr<odDb::Sound> sound;
    };

    // a slightly modified action variant with differents structs for storing prefetched animations and sounds
    using PlayerActionVariant = std::variant<PlayerActionStartAnim,
                                             PlayerActionPlaySound,
                                             odDb::ActionAttach,
                                             odDb::ActionRunStopAi,
                                             odDb::ActionShowHide,
                                             odDb::ActionMessage,
                                             odDb::ActionMusic>;

    class SequencePlayer
    {
    public:

        SequencePlayer(od::Level &level);

        /**
         * @brief Assigns a sequence and prepares the player for playing it, prefetching necessary assets.
         */
        void loadSequence(std::shared_ptr<odDb::Sequence> sequence);

        /**
         * @brief Starts playing the sequence.
         *
         * A sequence may state that all objects should be stopped, but since
         * the sequence player is run by an object's update loop, we have to
         * make an exception for the player. That's what the playerObject
         * parameter is for. The passed object will be excluded from the
         * deactivation step.
         */
        void play(od::LevelObject *playerObject);

        /**
         * @brief Returns false if no further updates are required i.e. the sequence has ended.
         */
        bool update(float relTime);


    private:

        struct Actor
        {
            Actor(od::LevelObject &o)
            : actorObject(o)
            {
            }

            od::LevelObject &actorObject;
            std::vector<odDb::ActionTransform> transformActions;
            std::vector<PlayerActionVariant> nonTransformActions;
        };

        friend class ActionLoadVisitor;

        void _applySingleKeyframe(od::LevelObject &obj, const odDb::ActionTransform &kf);
        void _applyInterpolatedKeyframes(od::LevelObject &obj, const odDb::ActionTransform &left, const odDb::ActionTransform &right);

        od::Level &mLevel;

        std::shared_ptr<odDb::Sequence> mSequence;
        float mSequenceTime;

        std::unordered_map<od::LevelObjectId, Actor> mActors;

    };

}

#endif
