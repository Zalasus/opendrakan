/*
 * @file StateManager.h
 * @date 21 Sep 2019
 * @author zal
 */

#ifndef INCLUDE_ODCORE_STATE_STATEMANAGER_H_
#define INCLUDE_ODCORE_STATE_STATEMANAGER_H_

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <odCore/ObjectTransform.h>

#include <odCore/input/Keys.h>

#include <odCore/state/Event.h>
#include <odCore/state/Timeline.h>

namespace od
{
    class Level;
    class LevelObject;
}

namespace odState
{
    class StateManager
    {
    public:

        class RollbackGuard
        {
        public:

            RollbackGuard(StateManager &sm);
            RollbackGuard(RollbackGuard &&g);
            ~RollbackGuard();


        private:

            StateManager *mStateManager;
        };


        StateManager(od::Level &level);

        //void addClient();

        //void addActionEvent(odInput::ActionCode actionCode, bool down);
        void objectTransformed(od::LevelObject &object, const od::ObjectTransform &tf, TickNumber tick = CURRENT_TICK);
        void objectVisibilityChanged(od::LevelObject &object, bool visible, TickNumber tick = CURRENT_TICK);

        /**
         * @brief Commits all changes and events last pushed into one snapshot.
         */
        void commit();

        /**
         * @brief Moves the world into the state it had during the given tick.
         *
         * Will throw if the given tick number is no longer being held in memory.
         */
        RollbackGuard rollback(TickNumber tick);

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

        using StateTransitionMap = std::unordered_map<od::LevelObjectId, ObjectStateTransition>;

        void combine(TickNumber begin, TickNumber end, StateTransitionMap &set);


    private:

        friend class RollbackGuard;

        od::Level &mLevel;

        bool mIgnoreStateUpdates;

        /**
         * A set of state events that can take the level as loaded from the file
         * to the most recent committed tick. This is useful for new joining
         * clients as well as for creating savegames.
         *
         * This is basically the union of all state transitions that happened so far.
         */
        StateTransitionMap mBaseStateTransitionMap;

        /**
         * A set of state events that represents the tick currently being
         * calculated. Incoming changes of the current update loop are gathered
         * here before being moved to the timeline upon the next commit.
         */
        StateTransitionMap mNextStateTransitionMap;

        // TODO: there should probably just be a ring buffer of StateTransitionMaps and a separate timeline just for non-state events
        Timeline<EventVariant> mEvents;

    };

}

#endif /* INCLUDE_ODCORE_STATE_STATEMANAGER_H_ */
