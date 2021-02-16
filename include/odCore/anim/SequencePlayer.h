
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
    class BoneAccumulator;

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

        inline std::shared_ptr<odDb::Sequence> getCurrentSequence() const { return mSequence; }
        inline bool isPlaying() const { return mPlaying; }

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
         * @brief Stops the sequence and performs cleanup.
         *
         * This will restart all objects that have been stopped automatically.
         */
        void stop();

        /**
         * @brief Returns false if no further updates are required i.e. the sequence has ended.
         */
        bool update(float relTime);

        /**
         * @brief Advances the world to the final frame of the current sequence, then stops playback.
         *
         * Does nothing if no sequence is playing.
         */
        void skipSequence();


    private:

        struct Actor
        {
            Actor(od::LevelObjectId objId)
            : actorObjectId(objId)
            , lastAppliedKeyframe(nullptr)
            {
            }

            od::LevelObjectId actorObjectId;

            // Note: if the player is itself owned by an object, this pointer
            //  creates a reference cycle for as long as a sequence is playing.
            //  this is probably non-critical due to the unlikelyhood of a
            //  player being an actor and the temporary nature of the cycle.
            std::shared_ptr<od::LevelObject> actorObject;

            std::vector<odDb::ActionTransform> transformActions;
            std::vector<PlayerActionVariant> nonTransformActions;
            const odDb::ActionTransform* lastAppliedKeyframe; // dirty hack to prevent applying the same single kf over and over again, causing conflicts with anim accumulation

            // sometimes animations in sequences use root bone accumulation. for that, we need to provide
            //  a bone accumulator that directly applies translations to the object. however, we need to preserve
            //  any accumulator already present for when the sequence finishes, so we store them here, too.
            std::shared_ptr<BoneAccumulator> motionToPositionRootAccumulator;
            std::shared_ptr<BoneAccumulator> prevRootAccumulator;
            AxesBoneModes prevRootBoneModes;
        };

        friend class ActionLoadVisitor;
        friend class NonTransformApplyVisitor;

        void _applySingleKeyframe(Actor &actor, const odDb::ActionTransform &kf);
        void _applyInterpolatedKeyframes(Actor &actor, const odDb::ActionTransform &left, const odDb::ActionTransform &right);

        od::Level &mLevel;

        std::shared_ptr<odDb::Sequence> mSequence;
        float mSequenceTime;
        float mSequenceDuration;
        float mLastUpdateSequenceTime;

        std::unordered_map<od::LevelObjectId, Actor> mActors;

        bool mPlaying;

    };

}

#endif
