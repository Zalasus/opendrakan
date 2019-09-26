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

        Server(odDb::DbManager &dbManager);

        inline odRfl::RflManager &getRflManager() { return *mRflManager; }

        void run();


    private:

        odDb::DbManager mDbManager;

        std::unique_ptr<odPhysics::PhysicsSystem> mPhysicsSystem;
        std::unique_ptr<odRfl::RflManager> mRflManager;

        std::unique_ptr<Level> mLevel;

        bool mIsDone;

    };

}


#endif /* INCLUDE_ODCORE_SERVER_H_ */
