
#include <dragonRfl/classes/UserInterfaceLauncher.h>

#include <odCore/Client.h>
#include <odCore/Level.h>
#include <odCore/LevelObject.h>
#include <odCore/Units.h>

#include <odCore/render/Renderer.h>

#include <dragonRfl/RflDragon.h>
#include <dragonRfl/classes/TrackingCamera.h>

namespace dragonRfl
{

	UserInterfaceLauncherFields::UserInterfaceLauncherFields()
	: launchWhat(LaunchWhat::MAIN_MENU)
	{
	}

    void UserInterfaceLauncherFields::probeFields(odRfl::FieldProbe &probe)
    {
		probe("User Interface Launcher")
    		     (launchWhat, "Launch What?");
    }


    void UserInterfaceLauncher_Cl::onMessageReceived(od::LevelObject &sender, od::Message message)
	{
        auto &rfl = getRfl<DragonRfl>();

        switch(mFields.launchWhat)
        {
        case UserInterfaceLauncherFields::LaunchWhat::MAIN_MENU:
            Logger::verbose() << "UserInterfaceLauncher opening main menu";
            rfl.setShowMenu(true);
            break;

        case UserInterfaceLauncherFields::LaunchWhat::MAP_SCREEN:
            Logger::verbose() << "UserInterfaceLauncher opening map screen";
            break;

        case UserInterfaceLauncherFields::LaunchWhat::ROLL_CREDITS:
            Logger::verbose() << "UserInterfaceLauncher rolling credits";
            break;
        }
	}

}
