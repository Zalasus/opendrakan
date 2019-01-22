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
#include <odCore/Engine.h>
#include <odCore/Exception.h>

#include <odOsg/render/Renderer.h>
#include <odOsg/audio/SoundSystem.h>
#include <odOsg/InputListener.h>


static od::Engine *sEngine = nullptr;
static void handleSignal(int signal)
{
    if(signal == SIGINT)
    {
        if(sEngine != nullptr)
        {
            sEngine->setDone(true);
        }
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
        << "If no level file and no options are given, the default intro level is loaded." << std::endl
        << "The latter assumes the current directory to be the game root." << std::endl
        << std::endl;
}

int main(int argc, char **argv)
{
    od::Logger::getDefaultLogger().setOutputLogLevel(od::LogLevel::Info);

    odOsg::SoundSystem soundSystem;

    odOsg::Renderer osgRenderer;

    od::Engine engine;
    sEngine = &engine;

    engine.setRenderer(&osgRenderer);
    engine.setSoundSystem(&soundSystem);

    signal(SIGINT, &handleSignal);

    int c;
    bool freeLook = false;
    while((c = getopt(argc, argv, "vhc")) != -1)
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

        case '?':
            std::cout << "Unknown option -" << optopt << std::endl;
            printUsage();
            return 1;
        }
    }

    if(optind < argc)
    {
        od::FilePath initialLevel(argv[optind]);
        engine.setInitialLevelOverride(initialLevel.adjustCase());
    }

    osgRenderer.setFreeLook(freeLook);

    std::unique_ptr<odOsg::InputListener> inputListener;
    if(!freeLook)
    {
        // only create listener if freelook mode is not forced. else we might catch input events the manipulator needs
        inputListener = std::make_unique<odOsg::InputListener>(osgRenderer, engine.getInputManager());
    }

    try
    {
        engine.run();

    }catch(od::Exception &e)
    {
        Logger::error() << "Terminating due to fatal error: " << e.what();
    }

    sEngine = nullptr;

    return 0;
}


