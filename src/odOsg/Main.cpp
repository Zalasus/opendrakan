/*
 * Main.cpp
 *
 *  Created on: 13 Nov 2018
 *      Author: zal
 */

#include <signal.h>
#include <unistd.h>

#include <odCore/Logger.h>
#include <odCore/Engine.h>
#include <odCore/Exception.h>

#include <odOsg/Renderer.h>


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
        << "    -v         Increase verbosity of logger" << std::endl
        << "    -h         Display this message and exit" << std::endl
        << "If no level file and no options are given, the default intro level is loaded." << std::endl
        << "The latter assumes the current directory to be the game root." << std::endl
        << std::endl;
}

int main(int argc, char **argv)
{
    od::Engine engine;
    sEngine = &engine;

    signal(SIGINT, &handleSignal);

    Logger::LogLevel logLevel = Logger::LOGLEVEL_INFO;
    int c;
    while((c = getopt(argc, argv, "vh")) != -1)
    {
        switch(c)
        {
        case 'v':
            if(logLevel < Logger::LOGLEVEL_DEBUG)
            {
                logLevel = static_cast<Logger::LogLevel>(1 + static_cast<int>(logLevel)); // i know, yucky enum abuse
            }
            break;

        case 'h':
            printUsage();
            return 0;

        case '?':
            std::cout << "Unknown option -" << optopt << std::endl;
            printUsage();
            return 1;
        }
    }

    Logger::getDefaultLogger().setOutputLogLevel(logLevel);

    if(optind < argc)
    {
        od::FilePath initialLevel(argv[optind]);
        engine.setInitialLevelOverride(initialLevel.adjustCase());
    }

    odOsg::Renderer osgRenderer;
    engine.setRenderer(&osgRenderer);

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


