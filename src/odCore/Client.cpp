/*
 * Client.cpp
 *
 *  Created on: Oct 6, 2019
 *      Author: zal
 */

#include <odCore/Client.h>

#include <odCore/physics/bullet/BulletPhysicsSystem.h>

namespace od
{

    Client::Client(odDb::DbManager &dbManager, odRfl::RflManager &rflManager, odRender::Renderer &renderer)
    : mDbManager(dbManager)
    , mRflManager(rflManager)
    , mRenderer(renderer)
    {
        mPhysicsSystem = std::make_unique<odBulletPhysics::BulletPhysicsSystem>();
    }

    odDb::DbManager &Client::getDbManager()
    {
        return mDbManager;
    }

    odRfl::RflManager &Client::getRflManager()
    {
        return mRflManager;
    }

    odPhysics::PhysicsSystem &Client::getPhysicsSystem()
    {
        return mPhysicsSystem;
    }

    odInput::InputManager *Client::getInputManager()
    {
        return nullptr;
    }

    odRender::Renderer *Client::getRenderer()
    {
        return &mRenderer;
    }

    odAudio::SoundSystem *Client::getSoundSystem()
    {
        return nullptr;
    }

}

