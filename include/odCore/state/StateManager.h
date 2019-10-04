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

namespace od
{
    class LevelObject;
}

namespace odState
{
    class StateHolder;
    class Client;

    class StateManager
    {
    public:

        void addClient();

        void addKeyEvent(StateHolder &holder, odInput::Key key);

        void addLevelObjectMove(od::LevelObject &obj, const glm::vec3 &pos);
        void addLevelObjectRotate(od::LevelObject &obj, const glm::quat &rot);
        void addLevelObjectScale(od::LevelObject &obj, const glm::vec3 &scale);
        void addLevelObjectStateTransition(od::LevelObject &obj);

        /**
         * @brief Commits all changes and events last pushed into one snapshot.
         */
        void commit();

        /**
         * @brief Rolls back all tracked states to the given tick number.
         *
         * Will throw if the given tick number is no longer being held in memory.
         */
        void rollback(uint32_t tickNumber);

        /**
         * @brief Calculates the total lag the given client probably has, then rolls back the world by that time.
         *
         * After this returns, the world is in approximately the same state as the client experienced it during it's last update.
         * Thus, target checks etc. can be performed in a lag-compensated manner.
         *
         * Make sure to call endLagCompensation() when you are finished to go back to the current world state.
         */
        void beginLagCompensation(Client *c);

        void endLagCompensation();


    private:

        struct ObjectChange
        {
            glm::vec3 pos;
            glm::quat rot;
            glm::vec3 scale;
        };

    };

}

#endif /* INCLUDE_ODCORE_STATE_STATEMANAGER_H_ */
