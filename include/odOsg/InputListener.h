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

        InputListener(Renderer &renderer, odInput::InputManager &inputManager);
        ~InputListener();

        bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);


    private:

        odInput::Key _osgKeyToOdKey(int key);

        Renderer &mRenderer;
        odInput::InputManager &mInputManager;

        osg::ref_ptr<osgGA::GUIEventHandler> mEventHandler;

    };

}

#endif /* INCLUDE_ODOSG_INPUTLISTENER_H_ */
