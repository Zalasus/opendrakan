/*
 * InputManager.cpp
 *
 *  Created on: 26 Feb 2018
 *      Author: zal
 */

#include "InputManager.h"

#include "Engine.h"
#include "Player.h"

namespace od
{

	InputManager::InputManager(Engine &engine, osgViewer::Viewer *viewer)
	: mEngine(engine)
	, mViewer(viewer)
	{
		mViewer->addEventHandler(this);
	}

	bool InputManager::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa, osg::Object *obj, osg::NodeVisitor *nv)
    {
        switch(ea.getEventType())
        {
        case osgGA::GUIEventAdapter::KEYDOWN:
            return this->_keyDown(ea.getKey());
        	break;

		case osgGA::GUIEventAdapter::KEYUP:
            return this->_keyUp(ea.getKey());
        	break;

		case osgGA::GUIEventAdapter::DRAG:
		case osgGA::GUIEventAdapter::MOVE:
			return this->_mouseMove(ea, aa);
			break;

        default:
            break;
        }

        return false;
    }

	bool InputManager::_keyDown(int key)
	{
		switch(key)
		{
		case osgGA::GUIEventAdapter::KEY_W:
			_playerMoveForward(1);
			return true;

		case osgGA::GUIEventAdapter::KEY_S:
			_playerMoveForward(-1);
			return true;

		case osgGA::GUIEventAdapter::KEY_A:
			_playerMoveRight(-1);
			return true;

		case osgGA::GUIEventAdapter::KEY_D:
			_playerMoveRight(1);
			return true;

		case osgGA::GUIEventAdapter::KEY_Escape:
			mViewer->setDone(true);
			break;

		default:
			break;
		}

		return false;
	}

	bool InputManager::_keyUp(int key)
	{
		switch(key)
		{
		case osgGA::GUIEventAdapter::KEY_W:
		case osgGA::GUIEventAdapter::KEY_S:
			_playerMoveForward(0);
			return true;

		case osgGA::GUIEventAdapter::KEY_A:
		case osgGA::GUIEventAdapter::KEY_D:
			_playerMoveRight(0);
			return true;

		case osgGA::GUIEventAdapter::KEY_F3:
			mEngine.getLevel().getPhysicsManager().toggleDebugDraw();
			return true;

		default:
			break;
		}

		return false;
	}

	bool InputManager::_mouseMove(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
	{
		if(mEngine.getPlayer() == nullptr)
		{
			return false;
		}

		float x = ea.getXnormalized();
		float y = ea.getYnormalized();

		float pitch = ( M_PI/2) * y;
		float yaw   = (-M_PI) * x;

		mEngine.getPlayer()->setPitch(pitch);
		mEngine.getPlayer()->setYaw(yaw);

		return true;
	}


	void InputManager::_playerMoveForward(float speed)
	{
		if(mEngine.getPlayer() != nullptr)
		{
			mEngine.getPlayer()->moveForward(speed);
		}
	}

	void InputManager::_playerMoveRight(float speed)
	{
		if(mEngine.getPlayer() != nullptr)
		{
			mEngine.getPlayer()->moveRight(speed);
		}
	}

}






