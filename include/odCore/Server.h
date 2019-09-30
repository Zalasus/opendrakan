/*
 * Server.h
 *
 *  Created on: Sep 24, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_SERVER_H_
#define INCLUDE_ODCORE_SERVER_H_

#include <memory>

namespace odDb
{
    class DbManager;
}

namespace odPhysics
{
    class PhysicsSystem;
}

namespace odRfl
{
    class RflManager;
}

namespace od
{
    class Level;

    class Server
    {
    public:

        Server(odDb::DbManager &dbManager, odRfl::RflManager &rflManager);

        inline odDb::DbManager &getDbManager() { return mDbManager; }
        inline odRfl::RflManager &getRflManager() { return mRflManager; }
        inline odPhysics::PhysicsSystem &getPhysicsSystem() { return *mPhysicsSystem; }

        void run();


    private:

        odDb::DbManager &mDbManager;
        odRfl::RflManager &mRflManager;

        std::unique_ptr<odPhysics::PhysicsSystem> mPhysicsSystem;

        std::unique_ptr<Level> mLevel;

        bool mIsDone;

    };

}


#endif /* INCLUDE_ODCORE_SERVER_H_ */