/*
 * InputListener.cpp
 *
 *  Created on: 30 Dec 2018
 *      Author: zal
 */


#include <odOsg/InputListener.h>

#include <cassert>

#include <odCore/input/InputManager.h>

#include <odOsg/render/Renderer.h>

namespace odOsg
{

    class EventHandler : public osgGA::GUIEventHandler
    {
    public:

        EventHandler(InputListener &il)
        : mInputListener(il)
        {
        }

        virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor*) override
        {
            return mInputListener.handle(ea, aa);
        }

    private:

        InputListener &mInputListener;

    };


    InputListener::InputListener(Renderer &renderer, odInput::InputManager &inputManager, bool consumeEvents)
    : mRenderer(renderer)
    , mInputManager(inputManager)
    , mConsumeEvents(consumeEvents)
    , mMouseWarped(false)
    {
        assert(mRenderer.getViewer() != nullptr);

        mEventHandler = new EventHandler(*this);
        mRenderer.getViewer()->addEventHandler(mEventHandler);
    }

    InputListener::~InputListener()
    {
        if(mRenderer.getViewer() != nullptr)
        {
            mRenderer.getViewer()->removeEventHandler(mEventHandler);
        }
    }

    bool InputListener::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
    {
        switch(ea.getEventType())
        {
        case osgGA::GUIEventAdapter::KEYDOWN:
            mInputManager.injectKey(_osgKeyToOdKey(ea.getKey()), true);
            return mConsumeEvents;

        case osgGA::GUIEventAdapter::KEYUP:
            mInputManager.injectKey(_osgKeyToOdKey(ea.getKey()), false);
            return mConsumeEvents;

        case osgGA::GUIEventAdapter::PUSH:
            if(ea.getButton() & osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) mInputManager.injectKey(odInput::Key::Mouse_Left, true);
            if(ea.getButton() & osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON) mInputManager.injectKey(odInput::Key::Mouse_Right, true);
            if(ea.getButton() & osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON) mInputManager.injectKey(odInput::Key::Mouse_Middle, true);
            return mConsumeEvents;

        case osgGA::GUIEventAdapter::RELEASE:
            if(ea.getButton() & osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) mInputManager.injectKey(odInput::Key::Mouse_Left, false);
            if(ea.getButton() & osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON) mInputManager.injectKey(odInput::Key::Mouse_Right, false);
            if(ea.getButton() & osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON) mInputManager.injectKey(odInput::Key::Mouse_Middle, false);
            return mConsumeEvents;

        case osgGA::GUIEventAdapter::MOVE:
        case osgGA::GUIEventAdapter::DRAG:
            _handleMouseMove(ea, aa);
            return mConsumeEvents;

        default:
            break;

        }

        return false;
    }

    void InputListener::_handleMouseMove(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
    {
        float x = ea.getXnormalized();
        float y = ea.getYnormalized();

        osg::Vec4 posNdc(x, y, 0, 1);
        osg::Vec4 posGui = posNdc * mRenderer.getNdcToGuiSpaceTransform();

        mInputManager.injectMouseMovement(posGui.x(), posGui.y());

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

    odInput::Key InputListener::_osgKeyToOdKey(int key)
    {
        // since OD adapted the key enum values from OSG, we can simply cast here
        return static_cast<odInput::Key>(key);
    }
}
