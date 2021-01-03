/*
 * InputListener.h
 *
 *  Created on: 30 Dec 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_INPUTLISTENER_H_
#define INCLUDE_ODOSG_INPUTLISTENER_H_

#include <osgGA/GUIEventHandler>

#include <odCore/input/Keys.h>

namespace odInput
{
    class InputManager;
}

namespace odOsg
{
    class Renderer;

    class InputListener
    {
    public:

        /**
         * @param consumeEvents  If true, the input listener will prevent events from being handled by any other OSG GUIEventHandler
         */
        InputListener(Renderer &renderer, odInput::InputManager &inputManager, bool consumeEvents);
        ~InputListener();

        bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);


    private:

        void _handleMouseMove(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

        odInput::Key _osgKeyToOdKey(int key);

        Renderer &mRenderer;
        odInput::InputManager &mInputManager;
        bool mConsumeEvents;

        bool mMouseWarped;

        osg::ref_ptr<osgGA::GUIEventHandler> mEventHandler;

    };

}

#endif /* INCLUDE_ODOSG_INPUTLISTENER_H_ */
