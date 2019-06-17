/*
 * Engine.cpp
 *
 *  Created on: 14 Feb 2018
 *      Author: zal
 */


#include <odCore/Engine.h>

#include <chrono>
#include <thread>

#include <odCore/OdDefines.h>
#include <odCore/Exception.h>
#include <odCore/Logger.h>
#include <odCore/Level.h>

#include <odCore/input/InputManager.h>

#include <odCore/rfl/RflManager.h>

#include <odCore/render/Renderer.h>

#include <odCore/audio/SoundSystem.h>

#include <odCore/physics/bullet/BulletPhysicsSystem.h>

#include <odCore/audio/music/MusicContainer.h>

namespace od
{

	Engine::Engine()
	: mRenderer(nullptr)
	, mSoundSystem(nullptr)
    , mHasInitialLevelOverride(false)
	, mInitialLevelOverride("")
	, mEngineRootDir("")
	, mSetUp(false)
	, mIsDone(false)
	{
        mInputManager = std::make_unique<odInput::InputManager>();
	}

	Engine::~Engine()
	{
	}

	odPhysics::PhysicsSystem &Engine::getPhysicsSystem()
	{
	    if(mPhysicsSystem == nullptr)
	    {
	        throw Exception("Engine has no physics system");
	    }

	    return *mPhysicsSystem;
	}

	void Engine::setRenderer(odRender::Renderer *renderer)
	{
	    if(mRenderer != nullptr)
	    {
	        Logger::warn() << "Hotswapping renderer. This might have undesired side-effects";
	    }

	    mRenderer = renderer;

	    if(mRenderer != nullptr)
	    {
	        mRenderer->setRendererEventListener(this);
	    }
	}

	void Engine::setSoundSystem(odAudio::SoundSystem *soundSystem)
	{
	    if(mSoundSystem != nullptr)
	    {
	        Logger::warn() << "Hotswapping sound system. Prepare for chaos";
	    }

	    mSoundSystem = soundSystem;
	}

	void Engine::setUp()
	{
	    if(mSetUp)
	    {
	        return;
	    }

	    _findEngineRoot("Dragon.rrc");

	    odAudio::MusicContainer m(FilePath("Music.rrc", mEngineRootDir));

	    mDbManager = std::make_unique<odDb::DbManager>(*this);
        mRflManager = std::make_unique<odRfl::RflManager>(*this);
        mPhysicsSystem = std::make_unique<odBulletPhysics::BulletPhysicsSystem>(*this);

	    mRflManager->onStartup();

	    mSetUp = true;
	}

	void Engine::run()
	{
		Logger::info() << "Starting OpenDrakan...";

		if(!mSetUp)
		{
		    setUp();
		}

		if(hasInitialLevelOverride())
        {
		    FilePath levelPath(getInitialLevelOverride());
            loadLevel(levelPath.adjustCase());
        }

		Logger::verbose() << "Everything set up. Starting main loop";

		if(mRenderer != nullptr)
		{
		    mRenderer->onStart();
		}

		float targetUpdateIntervalUs = (1e6/60.0);
		auto targetUpdateInterval = std::chrono::microseconds((int64_t)targetUpdateIntervalUs);
		auto lastUpdateStartTime = std::chrono::high_resolution_clock::now();
		while(!mIsDone)
		{
		    auto loopStart = std::chrono::high_resolution_clock::now();

		    double relTime = 1e-6 * std::chrono::duration_cast<std::chrono::microseconds>(loopStart - lastUpdateStartTime).count();
            lastUpdateStartTime = loopStart;

		    if(mLevel != nullptr)
		    {
		        mLevel->update(relTime);
		    }

		    mPhysicsSystem->update(relTime);

		    auto loopEnd = std::chrono::high_resolution_clock::now();
		    auto loopTime = loopEnd - loopStart;
		    if(loopTime < targetUpdateInterval)
		    {
		        std::this_thread::sleep_for(targetUpdateInterval - loopTime);

		    }else
		    {
		        float loopTimeMs = 1e-3 * std::chrono::duration_cast<std::chrono::microseconds>(loopTime).count();
		        Logger::warn() << "Update took too long. Took " << loopTimeMs << "ms, target update interval was " << (targetUpdateIntervalUs*1e-3) << "ms";
		    }
		}

		Logger::info() << "Shutting down gracefully";

		if(mRenderer != nullptr)
        {
            mRenderer->onEnd();
        }
	}

	void Engine::loadLevel(const FilePath &levelFile)
	{
	    if(mLevel != nullptr)
	    {
	        mLevel = nullptr;
	    }

	    mLevel = std::make_unique<od::Level>(levelFile, *this);
        mLevel->loadLevel();

        mLevel->spawnAllObjects();
	}

	void Engine::onRenderWindowClosed()
	{
	    if(mRenderer == nullptr)
	    {
	        return;
	    }

	    setDone(true);
	}

	void Engine::_findEngineRoot(const std::string &rrcFileName)
	{
	    // if we have been passed a level override, we need to find the engine root in that path.
	    //  if not, assume the engine root is the current working directory.
	    if(!hasInitialLevelOverride())
	    {
	        mEngineRootDir = FilePath(".");
	        Logger::verbose() << "No level override found on command line. Assuming we are running in engine root";
	        return;
	    }

	    // ascend in the passed initial level file path until we find a Dragon.rrc
        FilePath path = FilePath(rrcFileName, getInitialLevelOverride().dir()).adjustCase();
        while(!path.exists() && path.depth() > 1)
        {
            path = FilePath(rrcFileName, path.dir().dir()).adjustCase();
        }

        if(!path.exists())
        {
            Logger::error() << "Could not find engine root in passed level path. "
                    << "Make sure your level is located in the same directory or a subdirectory of " << rrcFileName;
            throw Exception("Could not find engine root in passed level path");
        }

        mEngineRootDir = path.dir();
        Logger::verbose() << "Found engine root here: " << mEngineRootDir.str();
	}

}
