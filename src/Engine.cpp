/*
 * Engine.cpp
 *
 *  Created on: 14 Feb 2018
 *      Author: zal
 */


#include "Engine.h"

#include "Exception.h"
#include "Logger.h"
#include "Level.h"
#include "LevelObject.h"
#include "Player.h"
#include "rfl/Rfl.h"

namespace od
{

	Engine::Engine()
	: mDbManager(*this)
	, mShaderManager(*this, FilePath("shader_src"))
	, mInitialLevelFile("Mountain World/Intro Level/Intro.lvl") // is this defined anywhere? EDIT: yes, it is. in the interface class db
	, mEngineRootDir("")
	, mCamera(nullptr)
	, mMaxFrameRate(60)
	{
	}

	void Engine::run()
	{
		Logger::info() << "Starting OpenDrakan...";

		odRfl::Rfl &rfl = odRfl::Rfl::getSingleton();
		Logger::info() << "OpenDrakan linked against RFL " << rfl.getName() << " with " << rfl.getClassTypeCount() << " registered classes";

		_findEngineRoot("Dragon.rrc");

		osg::ref_ptr<osg::Group> rootNode(new osg::Group);

		mViewer = new osgViewer::Viewer;
		mViewer->realize();
		mViewer->setName("OpenDrakan");
		mViewer->getCamera()->setClearColor(osg::Vec4(0.2,0.2,0.2,1));
		mViewer->setSceneData(rootNode);
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

		// attach our default shaders to root node so we don't use the fixed function pipeline anymore
		osg::ref_ptr<osg::Program> defaultProgram = mShaderManager.makeProgram(nullptr, nullptr); // nullptr will cause SM to load default shader
		rootNode->getOrCreateStateSet()->setAttribute(defaultProgram);

		osg::ref_ptr<osgViewer::StatsHandler> statsHandler(new osgViewer::StatsHandler);
		statsHandler->setKeyEventPrintsOutStats(osgGA::GUIEventAdapter::KEY_F2);
		statsHandler->setKeyEventTogglesOnScreenStats(osgGA::GUIEventAdapter::KEY_F1);
		mViewer->addEventHandler(statsHandler);

        mInputManager = new InputManager(*this, mViewer);


		mLevel.reset(new od::Level(mInitialLevelFile, *this, rootNode));
		mLevel->loadLevel();

		// level loaded. check whether all required interfaces are available through loaded RFL classes
		if(mLevel->getPlayer() == nullptr)
    	{
			Logger::error() << "Can't start engine. Level does not contain a Human Control object";
    		throw Exception("No HumanControl object present in level");
    	}

		if(mCamera == nullptr)
		{
		    Logger::error() << "Can't start engine. Level does not contain a camera object";
            throw Exception("No camera object present in level");

		}else
		{
		    mCamera->setOsgCamera(mViewer->getCamera());
		}

		mLevel->spawnAllObjects();


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

	void Engine::_findEngineRoot(const std::string &rrcFileName)
	{
	    // ascend in the passed initial level file path until we find a Dragon.rrc
        FilePath path = FilePath(rrcFileName, mInitialLevelFile.dir()).adjustCase();
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
