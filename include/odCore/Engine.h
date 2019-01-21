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

namespace od
{

    class Level;

	class Engine : public odRender::RendererEventListener
	{
	public:

	    Engine();
		Engine(Engine &e) = delete;
		Engine(const Engine &e) = delete;
		~Engine();

		inline bool hasInitialLevelOverride() const { return mHasInitialLevelOverride; }
		inline const FilePath &getInitialLevelOverride() const { return mInitialLevelOverride; }
		inline void setInitialLevelOverride(const FilePath &level) { mInitialLevelOverride = level; mHasInitialLevelOverride = true; }
		inline const FilePath &getEngineRootDir() const { return mEngineRootDir; }
		inline odDb::DbManager &getDbManager() { return *mDbManager; }
		inline odRfl::RflManager &getRflManager() { return *mRflManager; }
		inline odInput::InputManager &getInputManager() { return *mInputManager; }
		inline odRender::Renderer *getRenderer() { return mRenderer; }
		inline odAudio::SoundSystem *getSoundSystem() { return mSoundSystem; }
		inline Level &getLevel() { return *mLevel; } // FIXME: throw if no level present
		inline bool isDone() { return mIsDone; }
		inline void setDone(bool done) { mIsDone = done; }

		void setRenderer(odRender::Renderer *renderer);
		void setSoundSystem(odAudio::SoundSystem *soundSystem);

		void setUp();
		void run();

		void loadLevel(const FilePath &levelFile);

		virtual void onRenderWindowClosed() override;


	private:

		void _findEngineRoot(const std::string &rrcFileName);

		std::unique_ptr<odDb::DbManager> mDbManager;
		std::unique_ptr<odInput::InputManager> mInputManager;
		std::unique_ptr<odRfl::RflManager> mRflManager;
		odRender::Renderer *mRenderer;
		odAudio::SoundSystem *mSoundSystem;
		bool mHasInitialLevelOverride;
		FilePath mInitialLevelOverride;
		FilePath mEngineRootDir;
		std::unique_ptr<Level> mLevel;
		bool mSetUp;
		std::atomic_bool mIsDone;
	};

}

#endif /* INCLUDE_ENGINE_H_ */
