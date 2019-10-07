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

#include <odCore/Logger.h>
#include <odCore/Client.h>
#include <odCore/Server.h>
#include <odCore/Exception.h>
#include <odCore/FilePath.h>

#include <odCore/physics/PhysicsSystem.h>

#include <odCore/rfl/RflManager.h>

#include <odCore/db/DbManager.h>

#include <dragonRfl/RflDragon.h>

#include <odOsg/render/Renderer.h>
#include <odOsg/audio/SoundSystem.h>
#include <odOsg/InputListener.h>


static void handleSignal(int signal)
{
    if(signal == SIGINT)
    {
        Logger::info() << "Caught SIGINT. Can't handle that ATM. You need to kill the process manually, sorry.";
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

    //odOsg::SoundSystem soundSystem;
    odOsg::Renderer osgRenderer;

    odDb::DbManager dbManager;

    odRfl::RflManager rflManager;
    odRfl::Rfl &dragonRfl = rflManager.loadStaticRfl<dragonRfl::DragonRfl>(); // TODO: add option to specify dynamic RFL

    od::Client client(dbManager, rflManager, osgRenderer);
    od::Server server(dbManager, rflManager);

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
    if(optind < argc)
    {
        od::FilePath initialLevel(argv[optind]);
        if(!initialLevel.exists())
        {
            std::cerr << "Level file " << initialLevel << " does not exist" << std::endl;
        }
        //engine.setInitialLevelOverride(initialLevel.adjustCase());

        // if we have been passed a level override, we need to find the engine root in that path.
        //  if not, assume the engine root is the current working directory. TODO: add option to explicitly specify engine root
        engineRoot = findEngineRoot(initialLevel, "dragon.rrc");
    }

    client.setEngineRootDir(engineRoot);

    osgRenderer.setFreeLook(freeLook);

    std::unique_ptr<odOsg::InputListener> inputListener;
    if(!freeLook)
    {
        // only create listener if freelook mode is not forced. else we might catch input events the manipulator needs
        inputListener = std::make_unique<odOsg::InputListener>(osgRenderer, client.getInputManagerSafe());
    }

    dragonRfl.onGameStartup(server, client);

    if(physicsDebug)
    {
        client.getPhysicsSystem().setEnableDebugDrawing(true);
    }

    try
    {
        server.run();

    }catch(od::Exception &e)
    {
        Logger::error() << "Terminating due to fatal error: " << e.what();
    }

    return 0;
}


