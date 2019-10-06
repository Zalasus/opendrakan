/*
 * @file Client.h
 * @date 30 Sep 2019
 * @author zal
 */

#ifndef INCLUDE_ODCORE_CLIENT_H_
#define INCLUDE_ODCORE_CLIENT_H_

#include <odCore/Engine.h>

namespace odRender
{
    class Renderer;
}

namespace od
{

    class Client : public Engine
    {
    public:

        Client(odDb::DbManager &dbManager, odRfl::RflManager &rflManager, odRender::Renderer &renderer);

        /// @brief In contrast to getRenderer(), this returns a reference and is thus guaranteed to be non-null.
        inline odRender::Renderer &getRendererSafe() { return mRenderer; }

        virtual odDb::DbManager &getDbManager() override final;
        virtual odRfl::RflManager &getRflManager() override final;
        virtual odPhysics::PhysicsSystem &getPhysicsSystem() override final;
        virtual odInput::InputManager *getInputManager() override final;
        virtual odRender::Renderer *getRenderer() override final;
        virtual odAudio::SoundSystem *getSoundSystem() override final;


    private:

        odDb::DbManager &mDbManager;
        odRfl::RflManager &mRflManager;
        odRender::Renderer &mRenderer;

        std::unique_ptr<odPhysics::PhysicsSystem> mPhysicsSystem;

    };

}

#endif /* INCLUDE_ODCORE_CLIENT_H_ */
