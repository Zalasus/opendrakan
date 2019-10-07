/*
 * Engine.h
 *
 *  Created on: 14 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_ENGINE_H_
#define INCLUDE_ENGINE_H_

#include <memory>
#include <atomic>

#include <odCore/FilePath.h>

#include <odCore/render/RendererEventListener.h>

namespace odDb
{
    class DbManager;
}

namespace odInput
{
    class InputManager;
}

namespace odRfl
{
    class RflManager;
}

namespace odRender
{
    class Renderer;
}

namespace odAudio
{
    class SoundSystem;
}

namespace odPhysics
{
    class PhysicsSystem;
}

namespace od
{
    class Level;

    class Server;
    class Client;

    /**
     * @brief Interface for engine base objects that can provide engine subsystems.
     *
     * This will probably only be implemented by the Client and Server classes.
     *
     * TODO: Name is historically. We should rename this to something more expressive or remove it altogether.
     */
    class Engine
    {
    public:

        virtual ~Engine() = default;

        // mandatory subsystems
        virtual odDb::DbManager &getDbManager() = 0;
        virtual odRfl::RflManager &getRflManager() = 0;
        virtual odPhysics::PhysicsSystem &getPhysicsSystem() = 0;

        // optional (client-only) subsystems
        virtual odInput::InputManager *getInputManager() = 0;
        virtual odRender::Renderer *getRenderer() = 0;
        virtual odAudio::SoundSystem *getSoundSystem() = 0;
    };

}

#endif /* INCLUDE_ENGINE_H_ */
