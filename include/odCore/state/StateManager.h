/*
 * @file StateManager.h
 * @date 21 Sep 2019
 * @author zal
 */

#ifndef INCLUDE_ODCORE_STATE_STATEMANAGER_H_
#define INCLUDE_ODCORE_STATE_STATEMANAGER_H_

#include <mutex>
#include <unordered_map>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <odCore/ObjectTransform.h>

#include <odCore/input/Keys.h>

#include <odCore/state/Event.h>
#include <odCore/state/State.h>
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

        class CheckoutGuard
        {
        public:

            CheckoutGuard(StateManager &sm);
            CheckoutGuard(CheckoutGuard &&g);
            ~CheckoutGuard();


        private:

            StateManager *mStateManager;
        };

        using StateTransitionMap = std::unordered_map<od::LevelObjectId, ObjectStateTransition>;


        StateManager(od::Level &level);

        /**
         * @brief Returns the latest tick this state manager knows about.
         *
         * This is where the server should insert it's updates.
         */
        TickNumber getLatestTick() const;

        /**
         * @brief Returns the oldest tick this state manager knows about.
         */
        TickNumber getOldestTick() const;

        // note: these are synchronized internally. that might not be the correct place to to that
        void objectTransformed(od::LevelObject &object, const od::ObjectTransform &tf, TickNumber tick);
        void objectVisibilityChanged(od::LevelObject &object, bool visible, TickNumber tick);
        void objectCustomStateChanged(od::LevelObject &object, TickNumber tick);

        /**
         * @brief Advances to the next tick and merges the current one with the base state.
         */
        void advance();

        /**
         * @brief Advances until the given tick is stored in the timeline.
         */
        void advanceUntil(TickNumber tick);

        /**
         * @brief Applies the given tick to the level with optional interpolation.
         *
         * Use this carefully! The applied state will stay forever. Most likely,
         * checkout(...) is what you want to do instead.
         *
         * This supports interpolation between ticks. The lerp parameter can be
         * choosen in the range [0, inf), where 0 is exactly at the passed tick
         * and 1 is at the next tick.
         * TODO: dead reckoning can be implemented here, too, but we'd need to look back more than one tick (or use the base state)
         *
         * Will throw if the given tick number is not being held in memory.
         */
        void apply(TickNumber tick, float lerp = 0.0);

        /**
         * @brief Moves the world into the state it had/will have in the given tick.
         *
         * The level will remain in that state as long as the returned CheckoutGuard lives.
         *
         * Will throw if the given tick number is not being held in memory.
         */
        CheckoutGuard checkout(TickNumber tick);

        /**
         * @brief Sends all actions and state transitions of the given tick to the client connector.
         *
         * TODO: unify with apply() using a "StateApplier whatever" interface
         */
        void sendToClient(TickNumber tick, odNet::ClientConnector &c);


    private:

        StateTransitionMap &_getTransitionMapForTick(TickNumber tick);

        friend class CheckoutGuard;
        friend class ApplyGuard;

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

        std::mutex mUpdateMutex;

    };

}

#endif /* INCLUDE_ODCORE_STATE_STATEMANAGER_H_ */
