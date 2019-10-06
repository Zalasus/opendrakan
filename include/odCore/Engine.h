/*
 * Engine.h
 *
 *  Created on: 14 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_ENGINE_H_
#define INCLUDE_ENGINE_H_

#include <memory>
#include <atomic>

#include <odCore/FilePath.h>

#include <odCore/render/RendererEventListener.h>

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

namespace od
{
    class Level;

    class Server;
    class Client;

    enum class GameType
    {
        SP,
        MP_CLIENT,
        MP_HOST,
        MP_DEDICATED_SERVER
    };

    /**
     * @brief Interface for engine base objects that can provide engine subsystems.
     *
     * This will probably only be implemented by the Client and Server classes.
     *
     * TODO: Name is historically. We should rename this to something more expressive.
     */
    class Engine
    {
    public:

        virtual ~Engine() = default;

        // mandatory subsystems
        virtual odDb::DbManager &getDbManager() = 0;
        virtual odRfl::RflManager &getRflManager() = 0;
        virtual odPhysics::PhysicsSystem &getPhysicsSystem() = 0;

        // optional (client-only) subsystems
        virtual odInput::InputManager *getInputManager() = 0;
        virtual odRender::Renderer *getRenderer() = 0;
        virtual odAudio::SoundSystem *getSoundSystem() = 0;
    };


	class EngineImplOld : public odRender::RendererEventListener, public Engine
	{
	public:

	    EngineImplOld();
		virtual ~EngineImplOld();

		inline bool hasInitialLevelOverride() const { return mHasInitialLevelOverride; }
		inline const FilePath &getInitialLevelOverride() const { return mInitialLevelOverride; }
		inline void setInitialLevelOverride(const FilePath &level) { mInitialLevelOverride = level; mHasInitialLevelOverride = true; }
		inline const FilePath &getEngineRootDir() const { return mEngineRootDir; }
		inline Level &getLevel() { return *mLevel; } // FIXME: throw if no level present
		inline bool isDone() { return mIsDone; }
		inline void setDone(bool done) { mIsDone = done; }

        virtual odDb::DbManager &getDbManager() override;
        virtual odRfl::RflManager &getRflManager() override;
        virtual odPhysics::PhysicsSystem &getPhysicsSystem() override;
        virtual odInput::InputManager *getInputManager() override;
        virtual odRender::Renderer *getRenderer() override;
        virtual odAudio::SoundSystem *getSoundSystem() override;

		void setRenderer(odRender::Renderer *renderer);
		void setSoundSystem(odAudio::SoundSystem *soundSystem);

		void setUp();
		void run();

		void loadLevel(const FilePath &levelFile);

		virtual void onRenderWindowClosed() override;


	private:

		void _findEngineRoot(const std::string &rrcFileName);

		// intrinsic subsystems. these _must_ be initialized before, and destroyed after all other engine property
		std::unique_ptr<odDb::DbManager> mDbManager;
		std::unique_ptr<odInput::InputManager> mInputManager;
		std::unique_ptr<odRfl::RflManager> mRflManager;
		std::unique_ptr<odPhysics::PhysicsSystem> mPhysicsSystem;

		// external subsystems. these are managed from the outside, but still must live longer than this Engine object.
		odRender::Renderer *mRenderer;
		odAudio::SoundSystem *mSoundSystem;

		bool mHasInitialLevelOverride;
		FilePath mInitialLevelOverride;
		FilePath mEngineRootDir;
		std::unique_ptr<Level> mLevel;
		bool mSetUp;
		std::atomic_bool mIsDone;

		std::unique_ptr<Server> mLocalServer;
		std::unique_ptr<Client> mLocalClient;
	};

}

#endif /* INCLUDE_ENGINE_H_ */
