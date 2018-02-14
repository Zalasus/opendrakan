/*
 * Engine.cpp
 *
 *  Created on: 14 Feb 2018
 *      Author: zal
 */


#include "Engine.h"

#include <osgViewer/ViewerEventHandlers>

#include "Exception.h"
#include "Logger.h"
#include "Level.h"
#include "rfl/Rfl.h"

namespace od
{

	Engine::Engine()
	: mInitialLevelFile("Mountain World/Intro Level/Intro.lvl") // is this defined anywhere?
	{
	}

	void Engine::run()
	{
		Logger::info() << "Starting OpenDrakan...";

		Rfl &rfl = od::Rfl::getSingleton();
		Logger::info() << "OpenDrakan linked against RFL " << rfl.getName() << " with " << rfl.getClassTypeCount() << " registered classes";

		osg::ref_ptr<osg::Group> rootNode(new osg::Group);

		mViewer = new osgViewer::Viewer;
		mViewer->getCamera()->setClearColor(osg::Vec4(0.2,0.2,0.2,1));
		mViewer->setSceneData(rootNode);

		osg::ref_ptr<osgViewer::StatsHandler> statsHandler(new osgViewer::StatsHandler);
		statsHandler->setKeyEventPrintsOutStats('s');
		mViewer->addEventHandler(statsHandler);

		if(!mInitialLevelFile.exists())
		{
			Logger::error() << "Can't start engine. Initial level '" << mInitialLevelFile.str() << "' does not exist";
			throw Exception("Can't start engine. Initial level not set/does not exist");
		}
		mLevel.reset(new od::Level(mInitialLevelFile, *this, rootNode));

		mViewer->run();

		Logger::info() << "Shutting down gracefully";
	}

}
