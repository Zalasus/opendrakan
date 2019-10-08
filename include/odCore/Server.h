/*
 * Server.h
 *
 *  Created on: Sep 24, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_SERVER_H_
#define INCLUDE_ODCORE_SERVER_H_

#include <memory>
#include <atomic>

#include <odCore/FilePath.h>

namespace odDb
{
    class DbManager;
}

namespace odRfl
{
    class RflManager;
}

namespace odPhysics
{
    class PhysicsSystem;
}

namespace od
{
    class Level;

    /**
     * @brief Local server instance.
     *
     * There is no abstraction between a local and a remote server here! This class solely represents a
     * local server, which can be either a dedicated server, a listen server, or a singleplayer server.
     */
    class Server
    {
    public:

        Server(odDb::DbManager &dbManager, odRfl::RflManager &rflManager);
        ~Server();

        inline bool hasInitialLevelOverride() const { return mHasInitialLevelOverride; }
        inline const FilePath &getInitialLevelOverride() const { return mInitialLevelOverride; }
        inline void setInitialLevelOverride(const FilePath &level) { mInitialLevelOverride = level; mHasInitialLevelOverride = true; }
        inline void setIsDone(bool b) { mIsDone.store(b, std::memory_order::memory_order_relaxed); }

        inline odDb::DbManager &getDbManager() { return mDbManager; }
        inline odRfl::RflManager &getRflManager() { return mRflManager; }
        inline odPhysics::PhysicsSystem &getPhysicsSystem() { return *mPhysicsSystem; }

        void loadLevel(const od::FilePath &levelPath);

        void run();


    private:

        odDb::DbManager &mDbManager;
        odRfl::RflManager &mRflManager;

        std::unique_ptr<odPhysics::PhysicsSystem> mPhysicsSystem;
        std::unique_ptr<Level> mLevel;

        bool mHasInitialLevelOverride;
        FilePath mInitialLevelOverride;
        std::atomic_bool mIsDone;

    };

}


#endif /* INCLUDE_ODCORE_SERVER_H_ */
