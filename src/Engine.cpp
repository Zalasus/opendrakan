/*
 * Engine.cpp
 *
 *  Created on: 14 Feb 2018
 *      Author: zal
 */


#include "Engine.h"

#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>

#include "Exception.h"
#include "Logger.h"
#include "Level.h"
#include "LevelObject.h"
#include "Player.h"
#include "rfl/Rfl.h"

namespace od
{

	Engine::Engine()
	: mInitialLevelFile("Mountain World/Intro Level/Intro.lvl") // is this defined anywhere?
	, mMaxFrameRate(60)
	{
	}

	void Engine::run()
	{
		Logger::info() << "Starting OpenDrakan...";

		Rfl &rfl = od::Rfl::getSingleton();
		Logger::info() << "OpenDrakan linked against RFL " << rfl.getName() << " with " << rfl.getClassTypeCount() << " registered classes";

		osg::ref_ptr<osg::Group> rootNode(new osg::Group);

		mViewer = new osgViewer::Viewer;
		mViewer->setName("OpenDrakan");
		mViewer->getCamera()->setClearColor(osg::Vec4(0.2,0.2,0.2,1));
		mViewer->setSceneData(rootNode);

		mCamera.reset(new Camera(*this, mViewer->getCamera()));

		osg::ref_ptr<osgViewer::StatsHandler> statsHandler(new osgViewer::StatsHandler);
		statsHandler->setKeyEventPrintsOutStats(osgGA::GUIEventAdapter::KEY_F2);
		statsHandler->setKeyEventTogglesOnScreenStats(osgGA::GUIEventAdapter::KEY_F1);
		mViewer->addEventHandler(statsHandler);

		mLevel.reset(new od::Level(mInitialLevelFile, *this, rootNode));

		if(mLevel->getPlayer().getLevelObject() == nullptr)
    	{
			Logger::error() << "Can't start engine. Level does not contain a Human Control object";
    		throw Exception("No HumanControl object present in level");
    	}

		mInputManager = new InputManager(*this, mLevel->getPlayer(), mViewer);

		// need to provide our own loop as mViewer->run() installs camera manipulator we don't need
		mViewer->realize();
		double simTime = 0;
		while(!mViewer->done())
		{
			double minFrameTime = (mMaxFrameRate > 0.0) ? (1.0/mMaxFrameRate) : 0.0;
			osg::Timer_t startFrameTick = osg::Timer::instance()->tick();

			mViewer->advance(simTime);
			mViewer->eventTraversal();
			mViewer->updateTraversal();
			mViewer->renderingTraversals();

			osg::Timer_t endFrameTick = osg::Timer::instance()->tick();
			double frameTime = osg::Timer::instance()->delta_s(startFrameTick, endFrameTick);
			simTime += frameTime;
			if(frameTime < minFrameTime)
			{
				OpenThreads::Thread::microSleep(static_cast<unsigned int>(1000000.0*(minFrameTime-frameTime)));
			}
		}

		Logger::info() << "Shutting down gracefully";
	}

}
