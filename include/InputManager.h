/*
 * InputManager.h
 *
 *  Created on: 26 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_INPUTMANAGER_H_
#define INCLUDE_INPUTMANAGER_H_

#include <osgViewer/Viewer>
#include <osgGA/GUIEventHandler>

namespace od
{
	class Engine;
	class Player;

	class InputManager : public osgGA::GUIEventHandler
	{
	public:

		InputManager(Engine &engine, osgViewer::Viewer *viewer);

		virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa, osg::Object *obj, osg::NodeVisitor *nv) override;


	private:

		bool _keyDown(int key);
		bool _keyUp(int key);
		bool _mouseMove(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);
		void _playerMoveForward(float speed);
		void _playerMoveRight(float speed);

		Engine &mEngine;
		osg::ref_ptr<osgViewer::Viewer> mViewer;
		bool mMouseWarped; // flag to prevent handling mouseMove events caused by warping cursor
	};

}

#endif /* INCLUDE_INPUTMANAGER_H_ */
