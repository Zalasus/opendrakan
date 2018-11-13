/*
 * Main.cpp
 *
 *  Created on: 13 Nov 2018
 *      Author: zal
 */

#include <signal.h>

#include <odCore/Engine.h>

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


int main(int argc, char **argv)
{
    od::Engine engine;
    sEngine = &engine;

    signal(SIGINT, &handleSignal);

    if(argc >= 2)
    {
        od::FilePath initialLevel(argv[1]);
        engine.setInitialLevelOverride(initialLevel.adjustCase());
    }

    odOsg::Renderer osgRenderer;
    engine.setRenderer(&osgRenderer);

    engine.run();

    sEngine = nullptr;

    return 0;
}


