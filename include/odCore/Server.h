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
#include <odCore/FilePath.h>

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

    /**
     * @brief Local server instance.
     *
     * There is no abstraction between a local and a remote server here! This class solely represents a
     * local server, which can be either a dedicated server, a listen server, or a singleplayer server.
     */
    class Server : public Engine
    {
    public:

        Server(odDb::DbManager &dbManager, odRfl::RflManager &rflManager);
        virtual ~Server();

        inline bool hasInitialLevelOverride() const { return mHasInitialLevelOverride; }
        inline const FilePath &getInitialLevelOverride() const { return mInitialLevelOverride; }
        inline void setInitialLevelOverride(const FilePath &level) { mInitialLevelOverride = level; mHasInitialLevelOverride = true; }

        virtual odDb::DbManager &getDbManager() override final;
        virtual odRfl::RflManager &getRflManager() override final;
        virtual odPhysics::PhysicsSystem &getPhysicsSystem() override final;
        virtual odInput::InputManager *getInputManager() override final;
        virtual odRender::Renderer *getRenderer() override final;
        virtual odAudio::SoundSystem *getSoundSystem() override final;

        void loadLevel(const od::FilePath &levelPath);

        void run();


    private:

        odDb::DbManager &mDbManager;
        odRfl::RflManager &mRflManager;

        std::unique_ptr<odPhysics::PhysicsSystem> mPhysicsSystem;
        std::unique_ptr<Level> mLevel;

        bool mHasInitialLevelOverride;
        FilePath mInitialLevelOverride;
        bool mIsDone;

    };

}


#endif /* INCLUDE_ODCORE_SERVER_H_ */
