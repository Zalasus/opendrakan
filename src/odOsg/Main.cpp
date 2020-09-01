/*
 * Main.cpp
 *
 *  Created on: 13 Nov 2018
 *      Author: zal
 */

#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <memory>
#include <thread>

#include <odCore/Logger.h>
#include <odCore/Client.h>
#include <odCore/Server.h>
#include <odCore/Exception.h>
#include <odCore/FilePath.h>

#include <odCore/net/UplinkConnector.h>
#include <odCore/net/DownlinkConnector.h>

#include <odCore/physics/PhysicsSystem.h>

#include <odCore/rfl/RflManager.h>

#include <odCore/db/DbManager.h>

#include <dragonRfl/RflDragon.h>

#include <odOsg/render/Renderer.h>
#include <odOsg/audio/SoundSystem.h>
#include <odOsg/InputListener.h>

static od::Server *sServer = nullptr;
static od::Client *sClient = nullptr;
static void handleSignal(int signal)
{
    if(signal == SIGINT)
    {
        Logger::info() << "Caught SIGINT. Terminating server and client";

        if(sServer != nullptr) sServer->setIsDone(true);
        if(sClient != nullptr) sClient->setIsDone(true);
    }
}

static void printUsage()
{
    std::cout
        << "Usage: odOsg [options] [level file]" << std::endl
        << "OpenDrakan with OpenSceneGraph renderer" << std::endl
        << "Options:" << std::endl
        << "    -v  Increase verbosity of logger" << std::endl
        << "    -h  Display this message and exit" << std::endl
        << "    -c  Use free look trackball view and ignore in-game camera controllers" << std::endl
        << "    -p  Force enable physics debug drawing" << std::endl
        << "If no level file and no options are given, the default intro level is loaded." << std::endl
        << "The latter assumes the current directory to be the game root." << std::endl
        << std::endl;
}

static od::FilePath findEngineRoot(const od::FilePath &dir, const std::string &rrcFileName)
{
    // ascend in the passed directory until we find a Dragon.rrc
    od::FilePath path = od::FilePath(rrcFileName, dir).adjustCase();
    while(!path.exists() && path.depth() > 1)
    {
        path = od::FilePath(rrcFileName, path.dir().dir()).adjustCase();
    }

    if(!path.exists())
    {
        Logger::error() << "Could not find engine root in passed level path. "
                << "Make sure your level is located in the same directory or a subdirectory of " << rrcFileName;
        throw od::Exception("Could not find engine root in passed level path");
    }

    od::FilePath root = path.dir();
    Logger::verbose() << "Found engine root here: " << root;
    return root;
}

int main(int argc, char **argv)
{
    signal(SIGINT, &handleSignal);

    od::Logger::getDefaultLogger().setOutputLogLevel(od::LogLevel::Info);

    od::Logger::info() << "Starting OpenDrakan...";

    //odOsg::SoundSystem soundSystem;
    odOsg::Renderer osgRenderer;

    odDb::DbManager dbManager;

    odRfl::RflManager rflManager;
    odRfl::Rfl &dragonRfl = rflManager.loadStaticRfl<dragonRfl::DragonRfl>(); // TODO: add option to specify dynamic RFL

    od::Client client(dbManager, rflManager, osgRenderer);
    sClient = &client;

    od::Server server(dbManager, rflManager);
    auto clientId = server.addClient(client.getDownlinkConnector());
    sServer = &server;

    client.setUplinkConnector(server.getUplinkConnectorForClient(clientId));

    int c;
    bool freeLook = false;
    bool physicsDebug = false;
    while((c = getopt(argc, argv, "vhcp")) != -1)
    {
        switch(c)
        {
        case 'v':
            od::Logger::getDefaultLogger().increaseOutputLogLevel();
            break;

        case 'h':
            printUsage();
            return 0;

        case 'c':
            freeLook = true;
            break;

        case 'p':
            physicsDebug = true;
            break;

        case '?':
            std::cout << "Unknown option -" << optopt << std::endl;
            printUsage();
            return 1;
        }
    }

    od::FilePath engineRoot(".");
    od::FilePath initialLevelOverride;
    bool hasInitialLevelOverride = false;
    if(optind < argc)
    {
        initialLevelOverride = od::FilePath(argv[optind]);
        if(!initialLevelOverride.exists())
        {
            std::cerr << "Level file " << initialLevelOverride << " does not exist" << std::endl;

        }else
        {
            hasInitialLevelOverride = true;
        }

        // if we have been passed a level override, we need to find the engine root in that path.
        //  if not, assume the engine root is the current working directory. TODO: add option to explicitly specify engine root
        engineRoot = findEngineRoot(initialLevelOverride, "dragon.rrc");
    }

    client.setEngineRootDir(engineRoot);
    server.setEngineRootDir(engineRoot);

    osgRenderer.setFreeLook(freeLook);

    std::unique_ptr<odOsg::InputListener> inputListener;
    // if we use freelook mode, the input listener should not consume it's input events so the trackball can handle them, too
    bool consumeEvents = !freeLook;
    inputListener = std::make_unique<odOsg::InputListener>(osgRenderer, client.getInputManager(), consumeEvents);

    bool loadIntroLevel = !hasInitialLevelOverride;
    dragonRfl.onGameStartup(server, client, loadIntroLevel);

    if(hasInitialLevelOverride)
    {
        server.loadLevel(initialLevelOverride);
    }

    if(physicsDebug)
    {
        client.getPhysicsSystem().setEnableDebugDrawing(true);
    }

    auto serverThreadFunc = [&server, &client]()
    {
        try
        {
            server.run();

        }catch(od::Exception &e)
        {
            Logger::error() << "Terminating server due to fatal error: " << e.what();
        }

        client.setIsDone(true);
    };

    auto clientThreadFunc = [&client, &server]()
    {
        try
        {
            client.run();

        }catch(od::Exception &e)
        {
            Logger::error() << "Terminating client due to fatal error: " << e.what();
        }

        server.setIsDone(true);
    };

    std::thread serverThread(serverThreadFunc);

    // we run the client in the main thread for now, as there is always a client for the normal game
    clientThreadFunc();

    server.setIsDone(true); // clientThreadFunc() will not have set this if it terminated abnormally! (non-OD-exception)
    serverThread.join();

    sClient = nullptr;
    sServer = nullptr;

    return 0;
}
