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
	{
	}

	Camera &Engine::getCamera()
	{
		if(mCamera == nullptr)
		{
			if(mViewer == nullptr)
			{
				throw Exception("No camera available because no viewer created yet");
			}

			mCamera.reset(new Camera(mViewer->getCamera()));
		}

		return *mCamera.get();
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

		osg::ref_ptr<osgViewer::StatsHandler> statsHandler(new osgViewer::StatsHandler);
		statsHandler->setKeyEventPrintsOutStats(osgGA::GUIEventAdapter::KEY_F2);
		statsHandler->setKeyEventTogglesOnScreenStats(osgGA::GUIEventAdapter::KEY_F1);
		mViewer->addEventHandler(statsHandler);

		if(!mInitialLevelFile.exists())
		{
			Logger::error() << "Can't start engine. Initial level '" << mInitialLevelFile.str() << "' does not exist";
			throw Exception("Can't start engine. Initial level not set/does not exist");
		}
		mLevel.reset(new od::Level(mInitialLevelFile, *this, rootNode));

		if(mLevel->getPlayer().getLevelObject() == nullptr)
    	{
			Logger::error() << "Can't start engine. Level does not contain a Human Control object";
    		throw Exception("No HumanControl object present in level");
    	}

		mInputManager = new InputManager(*this, mLevel->getPlayer(), mViewer);

		osgGA::TrackballManipulator *manip(new osgGA::TrackballManipulator);
		manip->setHomePosition(mLevel->getPlayer().getLevelObject()->getPosition(), osg::Vec3(0,0,0), osg::Vec3(0,1,0));
		mViewer->setCameraManipulator(manip);

		mViewer->run();

		Logger::info() << "Shutting down gracefully";
	}

}
