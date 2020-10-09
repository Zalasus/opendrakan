
#ifndef INCLUDE_ODCORE_ANIM_SEQUENCEPLAYER_H_
#define INCLUDE_ODCORE_ANIM_SEQUENCEPLAYER_H_

#include <vector>

#include <odCore/db/Sequence.h>

namespace od
{
    class Level;
    class LevelObject;
}

namespace odAnim
{

    class SequencePlayer
    {
    public:

        SequencePlayer();

        /**
         * @brief Loads a sequence. Does not take ownership!
         */
        void setSequence(od::Level &level, odDb::Sequence &sequence);

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
            std::vector<odDb::ActionVariant> nonTransformActions;
        };

        float mSequenceTime;

        std::vector<Actor> mActors;

    };

}

#endif
