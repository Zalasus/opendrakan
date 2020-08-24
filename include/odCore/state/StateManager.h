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

namespace odNet
{
    class ClientConnector;
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

        using StateTransitionMap = std::unordered_map<od::LevelObjectId, ObjectStateTransition>;


        StateManager(od::Level &level);

        /**
         * @brief Returns the latest tick that is managed by this and can be written to.
         */
        TickNumber getMaxTick() const;

        /**
         * @brief Returns the latest tick not yet committed.
         */
        TickNumber getCurrentTick() const;

        void objectTransformed(od::LevelObject &object, const od::ObjectTransform &tf, TickNumber tick);
        void objectVisibilityChanged(od::LevelObject &object, bool visible, TickNumber tick);

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

        void sendToClient(TickNumber tick, odNet::ClientConnector &c);


    private:

        StateTransitionMap &_getTransitionMapForTick(TickNumber tick);
        StateTransitionMap &_getCurrentTransitionMap();

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

        size_t mOldestTickIndex;
        TickNumber mOldestTick;
        std::vector<StateTransitionMap> mStateTransitions;

        // TODO: this timeline's tick management is kinda redundant to the one the SM does itself. unify this, plz 
        Timeline<EventVariant> mEvents;

    };

}

#endif /* INCLUDE_ODCORE_STATE_STATEMANAGER_H_ */
