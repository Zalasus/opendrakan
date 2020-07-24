/*
 * @file Client.h
 * @date 30 Sep 2019
 * @author zal
 */

#ifndef INCLUDE_ODCORE_CLIENT_H_
#define INCLUDE_ODCORE_CLIENT_H_

#include <memory>
#include <atomic>

#include <odCore/FilePath.h>

namespace odDb
{
    class DbManager;
}

namespace odInput
{
    class InputManager;
}

namespace odRfl
{
    class RflManager;
}

namespace odRender
{
    class Renderer;
}

namespace odAudio
{
    class SoundSystem;
}

namespace odPhysics
{
    class PhysicsSystem;
}

namespace odNet
{
    class ClientConnector;
}

namespace odState
{
    class StateManager;
}

namespace od
{
    class Level;

    class Client
    {
    public:

        Client(odDb::DbManager &dbManager, odRfl::RflManager &rflManager, odRender::Renderer &renderer);
        ~Client();

        inline void setEngineRootDir(const od::FilePath &path) { mEngineRoot = path; }
        inline const od::FilePath &getEngineRootDir() const { return mEngineRoot; }
        inline void setIsDone(bool b) { mIsDone.store(b, std::memory_order_relaxed); }

        inline odDb::DbManager &getDbManager() { return mDbManager; }
        inline odRfl::RflManager &getRflManager() { return mRflManager; }
        inline odPhysics::PhysicsSystem &getPhysicsSystem() { return *mPhysicsSystem; }
        inline odInput::InputManager &getInputManager() { return *mInputManager; }
        inline odRender::Renderer &getRenderer() { return mRenderer; }
        inline odAudio::SoundSystem *getSoundSystem() { return nullptr; }
        inline odState::StateManager &getStateManager() { return *mStateManager; }

        inline od::Level *getLevel() { return mLevel.get(); }

        /**
         * @brief Creates a connector to connect this client to a local server.
         */
        std::unique_ptr<odNet::ClientConnector> createLocalConnector();

        void loadLevel(const FilePath &lvlPath);

        void run();


    private:

        odDb::DbManager &mDbManager;
        odRfl::RflManager &mRflManager;
        odRender::Renderer &mRenderer;

        std::unique_ptr<odPhysics::PhysicsSystem> mPhysicsSystem;
        std::unique_ptr<odInput::InputManager> mInputManager;
        std::unique_ptr<odState::StateManager> mStateManager;

        od::FilePath mEngineRoot;

        std::atomic_bool mIsDone;

        std::unique_ptr<Level> mLevel;

    };

}

#endif /* INCLUDE_ODCORE_CLIENT_H_ */
