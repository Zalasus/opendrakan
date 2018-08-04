/*
 * InputManager.cpp
 *
 *  Created on: 26 Feb 2018
 *      Author: zal
 */

#include "InputManager.h"

#include "Engine.h"
#include "Player.h"
#include "gui/GuiManager.h"

namespace od
{

	InputManager::InputManager(Engine &engine, osgViewer::Viewer *viewer)
	: mEngine(engine)
	, mViewer(viewer)
	, mMouseWarped(false)
	{
	    _initCursor();

		mViewer->addEventHandler(this);
	}

	bool InputManager::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa, osg::Object *obj, osg::NodeVisitor *nv)
    {
        switch(ea.getEventType())
        {
        case osgGA::GUIEventAdapter::KEYDOWN:
            return this->_keyDown(ea.getKey(), aa);
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

	bool InputManager::_keyDown(int key, osgGA::GUIActionAdapter &aa)
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
		    _toggleMainMenu(aa);
			return true;

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
	    mLastCursorPos.set(ea.getX(), ea.getY());
	    mLastCursorPosNorm.set(ea.getXnormalized(), ea.getYnormalized());

		if(mMouseWarped)
		{
			mMouseWarped = false;
			return true;
		}

		if(mEngine.getPlayer() == nullptr)
		{
			return false;
		}

		float x = ea.getXnormalized();
		float y = ea.getYnormalized();

		// set cursor position regardless of menu mode so the cursor dos not jump upon opening a menu
        mEngine.getGuiManager().setCursorPosition(osg::Vec2(x, y));

		if(!mEngine.getGuiManager().isMenuMode())
		{
		    // not menu mode. apply mouse movement to player view

		    float pitch = ( M_PI/2) * y;
            float yaw   = (-M_PI) * x;

            mEngine.getPlayer()->setPitch(pitch);
            mEngine.getPlayer()->setYaw(yaw);

            // wrap cursor when hitting left or right border
            float epsilon = 2.0/(ea.getXmax() - ea.getXmin()); // epsilon of one pixel
            if(x <= -1 + epsilon || x >= 1 - epsilon)
            {
                float newX = (x <= -1 + epsilon) ? (1 - 2*epsilon) : (-1 + 2*epsilon);

                float denormX = ea.getXmin() + (ea.getXmax() - ea.getXmin())*(newX+1.0)/2.0;

                mMouseWarped = true;
                aa.requestWarpPointer(denormX, ea.getY());
            }
		}

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

	void InputManager::_toggleMainMenu(osgGA::GUIActionAdapter &aa)
	{
        bool menuMode = mEngine.getGuiManager().isMenuMode();

        if(menuMode)
        {
            // leaving menu mode
            mLastMenuCursorPos = mLastCursorPos;
            mLastMenuCursorPosNorm = mLastCursorPosNorm;

            mLastCursorPos = mLastGameCursorPos;
            mLastCursorPosNorm = mLastGameCursorPosNorm;

        }else
        {
            // entering menu mode
            mLastGameCursorPos = mLastCursorPos;
            mLastGameCursorPosNorm = mLastCursorPosNorm;

            mLastCursorPos = mLastMenuCursorPos;
            mLastCursorPosNorm = mLastMenuCursorPosNorm;
        }

        mMouseWarped = true;
        aa.requestWarpPointer(mLastCursorPos.x(), mLastCursorPos.y());
        mEngine.getGuiManager().setCursorPosition(mLastCursorPosNorm);

        mEngine.getGuiManager().setShowMainMenu(!menuMode);
	}

	void InputManager::_initCursor()
	{
        osgViewer::Viewer::Windows windows;
        mViewer->getWindows(windows);
        if(windows.empty())
        {
            throw Exception("Could not init cursor. No windows found");
        }

        // hide cursor
        for(auto it = windows.begin(); it != windows.end(); ++it)
        {
            (*it)->setCursor(osgViewer::GraphicsWindow::NoCursor);
        }

        // init cursor positions to center of screen
        int windowPosX;
        int windowPosY;
        int windowWidth;
        int windowHeight;
        windows.back()->getWindowRectangle(windowPosX, windowPosY, windowWidth, windowHeight);

        float cursorX = windowPosX + windowWidth/2;
        float cursorY = windowPosY + windowHeight/2;

        mViewer->requestWarpPointer(cursorX, cursorY);

        mLastCursorPos.set(cursorX, cursorY);
        mLastCursorPosNorm.set(0, 0);
        mLastMenuCursorPos.set(cursorX, cursorY);
        mLastMenuCursorPosNorm.set(0, 0);
        mLastGameCursorPos.set(cursorX, cursorY);
        mLastGameCursorPosNorm.set(0, 0);
	}
}



