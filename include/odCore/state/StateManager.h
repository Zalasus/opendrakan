/*
 * @file StateManager.h
 * @date 21 Sep 2019
 * @author zal
 */

#ifndef INCLUDE_ODCORE_STATE_STATEMANAGER_H_
#define INCLUDE_ODCORE_STATE_STATEMANAGER_H_

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <odCore/input/Keys.h>

#include <odCore/state/Event.h>
#include <odCore/state/ObjectTransform.h>
#include <odCore/state/Timeline.h>

namespace od
{
    class LevelObject;
}

namespace odState
{
    class StateManager
    {
    public:

        StateManager();

        //void addClient();

        void addActionEvent(odInput::ActionCode actionCode, bool down);

        void addObjectTransform(od::LevelObject &object, const ObjectTransform &tf);

        /**
         * @brief Commits all changes and events last pushed into one snapshot.
         */
        //void commit();

        /**
         * @brief Rolls back all tracked states to the given tick number.
         *
         * Will throw if the given tick number is no longer being held in memory.
         */
        //void rollback(TickNumber tick);

        void apply(TickNumber tick);

        /**
         * @brief Calculates the total lag the given client probably has, then rolls back the world by that time.
         *
         * After this returns, the world is in approximately the same state as the client experienced it during it's last update.
         * Thus, target checks etc. can be performed in a lag-compensated manner.
         *
         * Make sure to call endLagCompensation() when you are finished to go back to the current world state.
         */
        //void beginLagCompensation(Client *c);

        //void endLagCompensation();


    private:

        Timeline<EventVariant> mEvents;

    };

}

#endif /* INCLUDE_ODCORE_STATE_STATEMANAGER_H_ */
