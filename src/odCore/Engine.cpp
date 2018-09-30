/*
 * Engine.cpp
 *
 *  Created on: 14 Feb 2018
 *      Author: zal
 */


#include <odCore/Engine.h>

#include <odCore/OdDefines.h>
#include <odCore/Exception.h>
#include <odCore/Logger.h>
#include <odCore/Level.h>
#include <odCore/Player.h>
#include <odCore/Camera.h>
#include <odCore/ShaderManager.h>
#include <odCore/InputManager.h>
#include <odCore/light/LightManager.h>
#include <odCore/gui/GuiManager.h>
#include <odCore/rfl/RflManager.h>
#include <odCore/audio/SoundManager.h>

namespace od
{

	Engine::Engine()
	: mHasInitialLevelOverride(false)
	, mInitialLevelOverride("")
	, mEngineRootDir("")
	, mCamera(nullptr)
	, mPlayer(nullptr)
	, mMaxFrameRate(60)
	, mSetUp(false)
	{
	}

	Engine::~Engine()
	{
	}

	void Engine::setUp()
	{
	    if(mSetUp)
	    {
	        return;
	    }

	    _findEngineRoot("Dragon.rrc");

	    mDbManager.reset(new DbManager(*this));
	    mShaderManager.reset(new ShaderManager(*this, FilePath(OD_SHADER_SRC_PATH)));
	    mRflManager.reset(new RflManager(*this));

	    mViewer = new osgViewer::Viewer;
        mViewer->realize();
        mViewer->setName("OpenDrakan");
        mViewer->getCamera()->setClearColor(osg::Vec4(0.2,0.2,0.2,1));
        mViewer->setKeyEventSetsDone(0); // we handle the escape key ourselves

        // set window title
        osgViewer::Viewer::Windows windows;
        mViewer->getWindows(windows);
        if(!windows.empty())
        {
            windows.back()->setWindowName(mViewer->getName());
        }

        osg::ref_ptr<osgViewer::ScreenCaptureHandler::CaptureOperation> captureOp =
                new osgViewer::ScreenCaptureHandler::WriteToFile("screenshot", "png", osgViewer::ScreenCaptureHandler::WriteToFile::SEQUENTIAL_NUMBER);
        mScreenshotHandler = new osgViewer::ScreenCaptureHandler(captureOp, 1);
        mScreenshotHandler->setKeyEventTakeScreenShot(osgGA::GUIEventAdapter::KEY_F12);
        mViewer->addEventHandler(mScreenshotHandler);

        osg::ref_ptr<osgViewer::StatsHandler> statsHandler(new osgViewer::StatsHandler);
        statsHandler->setKeyEventPrintsOutStats(osgGA::GUIEventAdapter::KEY_F2);
        statsHandler->setKeyEventTogglesOnScreenStats(osgGA::GUIEventAdapter::KEY_F1);
        mViewer->addEventHandler(statsHandler);

        mRootNode = new osg::Group();
        mViewer->setSceneData(mRootNode);

        // attach our default shaders to root node so we don't use the fixed function pipeline anymore
        osg::ref_ptr<osg::Program> defaultProgram = mShaderManager->makeProgram(nullptr, nullptr); // nullptr will cause SM to load default shader
        mRootNode->getOrCreateStateSet()->setAttribute(defaultProgram);

        mGammaUniform = new osg::Uniform("fullScreenGamma", OD_DEFAULT_GAMMA);
        osg::StateSet *rootStateSet = mRootNode->getOrCreateStateSet();
        rootStateSet->addUniform(mGammaUniform, osg::StateAttribute::ON);
        rootStateSet->setMode(GL_FRAMEBUFFER_SRGB, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

        mInputManager = new InputManager(*this, mViewer);

	    mLightManager.reset(new LightManager(*this, mRootNode));

	    mGuiManager.reset(new GuiManager(*this, mViewer));

	    std::vector<std::string> devs;
	    SoundManager::listDeviceNames(devs);
	    for(std::string s : devs)
	    {
	        Logger::info() << "Sound device: '" << s << "'";
	    }
	    mSoundManager.reset(new SoundManager(nullptr));

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

		// need to provide our own loop as mViewer->run() installs camera manipulator we don't need
		double simTime = 0;
		double frameTime = 0;
		while(!mViewer->done())
		{
			double minFrameTime = (mMaxFrameRate > 0.0) ? (1.0/mMaxFrameRate) : 0.0;
			osg::Timer_t startFrameTick = osg::Timer::instance()->tick();

			mViewer->advance(simTime);
			mViewer->eventTraversal();

			if(mLevel != nullptr)
			{
			    mLevel->update(); // TODO: put in update callback
			}

			mViewer->updateTraversal();
			mViewer->renderingTraversals();

			osg::Timer_t endFrameTick = osg::Timer::instance()->tick();
			frameTime = osg::Timer::instance()->delta_s(startFrameTick, endFrameTick);
			simTime += frameTime;
			if(frameTime < minFrameTime)
			{
				simTime += (minFrameTime-frameTime);
				OpenThreads::Thread::microSleep(static_cast<unsigned int>(1000000.0*(minFrameTime-frameTime)));
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

	    mLevel.reset(new od::Level(levelFile, *this, mRootNode));
        mLevel->loadLevel();

        if(mCamera != nullptr)
        {
            mCamera->setOsgCamera(mViewer->getCamera());
        }

        mLevel->spawnAllObjects();
	}

	void Engine::setFullScreenGamma(float gamma)
	{
	    if(mGammaUniform == nullptr)
	    {
	        return;
	    }

	    mGammaUniform->set(gamma);
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
