/*
 * Server.h
 *
 *  Created on: Sep 24, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_SERVER_H_
#define INCLUDE_ODCORE_SERVER_H_

#include <memory>

#include <odCore/Engine.h>

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

    class Server : public Engine
    {
    public:

        Server(odDb::DbManager &dbManager, odRfl::RflManager &rflManager);

        virtual odDb::DbManager &getDbManager() override final;
        virtual odRfl::RflManager &getRflManager() override final;
        virtual odPhysics::PhysicsSystem &getPhysicsSystem() override final;
        virtual odInput::InputManager *getInputManager() override final;
        virtual odRender::Renderer *getRenderer() override final;
        virtual odAudio::SoundSystem *getSoundSystem() override final;

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
