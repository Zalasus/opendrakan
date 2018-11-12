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
#include <odCore/rfl/RflManager.h>

namespace od
{

	Engine::Engine()
	: mRenderer(nullptr)
    , mHasInitialLevelOverride(false)
	, mInitialLevelOverride("")
	, mEngineRootDir("")
	, mSetUp(false)
	, mKeepRunning(true)
	{
	}

	Engine::~Engine()
	{
	}

	void Engine::setRenderer(odRender::Renderer *renderer)
	{
	    if(mRenderer != nullptr)
	    {
	        Logger::warn() << "Hotswapping renderer. This might have undesired side-effects";
	    }

	    mRenderer = renderer;
	}

	void Engine::setUp()
	{
	    if(mSetUp)
	    {
	        return;
	    }

	    _findEngineRoot("Dragon.rrc");

	    mDbManager.reset(new odDb::DbManager(*this));
        mRflManager.reset(new odRfl::RflManager(*this));

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

		auto targetUpdateInterval = std::chrono::microseconds((int64_t)(1e6/60.0));
		auto lastUpdateStartTime = std::chrono::high_resolution_clock::now();
		while(mKeepRunning)
		{
		    auto loopStart = std::chrono::high_resolution_clock::now();

		    if(mLevel != nullptr)
		    {
		        double relTime = 1e-6 * std::chrono::duration_cast<std::chrono::microseconds>(loopStart - lastUpdateStartTime).count();
		        lastUpdateStartTime = loopStart;

		        mLevel->update(relTime);
		    }

		    auto loopEnd = std::chrono::high_resolution_clock::now();
		    auto loopTime = loopEnd - loopStart;
		    if(loopTime < targetUpdateInterval)
		    {
		        std::this_thread::sleep_for(targetUpdateInterval - loopTime);
		    }
		}

		Logger::info() << "Shutting down gracefully";
	}

	void Engine::loadLevel(const FilePath &levelFile)
	{
	    if(mLevel != nullptr)
	    {
	        mLevel = nullptr;
	    }

	    mLevel.reset(new od::Level(levelFile, *this));
        mLevel->loadLevel();

        mLevel->spawnAllObjects();
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
