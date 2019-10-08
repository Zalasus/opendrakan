/*
 * Client.cpp
 *
 *  Created on: Oct 6, 2019
 *      Author: zal
 */

#include <odCore/Client.h>

#include <chrono>

#include <odCore/render/Renderer.h>

#include <odCore/input/InputManager.h>

#include <odCore/physics/bullet/BulletPhysicsSystem.h>

namespace od
{

    Client::Client(odDb::DbManager &dbManager, odRfl::RflManager &rflManager, odRender::Renderer &renderer)
    : mDbManager(dbManager)
    , mRflManager(rflManager)
    , mRenderer(renderer)
    , mEngineRoot(".")
    , mIsDone(false)
    {
        mPhysicsSystem = std::make_unique<odBulletPhysics::BulletPhysicsSystem>(&renderer);
        mInputManager = std::make_unique<odInput::InputManager>();
    }

    Client::~Client()
    {
    }

    void Client::run()
    {
        Logger::info() << "OpenDrakan client starting...";

        mRenderer.setup();

        Logger::info() << "Client set up. Starting render loop";

        using namespace std::chrono;
        auto lastTime = high_resolution_clock::now();
        while(!mIsDone.load(std::memory_order_relaxed))
        {
            auto duration = duration_cast<microseconds>(high_resolution_clock::now() - lastTime);
            float relTime = duration.count() * 1e-6;
            mRenderer.frame(relTime);
        }

        Logger::info() << "Shutting down client gracefully";
    }

}

