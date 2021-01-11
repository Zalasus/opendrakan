
#ifndef INCLUDE_RFL_DRAGON_USERINTERFACELAUNCHER_H_
#define INCLUDE_RFL_DRAGON_USERINTERFACELAUNCHER_H_

#include <odCore/rfl/Class.h>
#include <odCore/rfl/DummyClass.h>
#include <odCore/rfl/Field.h>

namespace dragonRfl
{

    struct UserInterfaceLauncherFields final : public odRfl::FieldBundle
    {
        UserInterfaceLauncherFields();

        virtual void probeFields(odRfl::FieldProbe &probe) override;

        enum class LaunchWhat
        {
            MAIN_MENU,
            MAP_SCREEN,
            ROLL_CREDITS
        };

        odRfl::EnumImpl<LaunchWhat, 0, 2> launchWhat;
    };


    class UserInterfaceLauncher_Cl final : public odRfl::ClientClass, public odRfl::SpawnableClass, public odRfl::ClassImpl<UserInterfaceLauncher_Cl>
    {
    public:

        virtual odRfl::FieldBundle &getFields() override { return mFields; }

        virtual void onMessageReceived(od::LevelObject &sender, od::Message message) override;


    private:

        UserInterfaceLauncherFields mFields;

    };


    using UserInterfaceLauncherFactory = odRfl::DefaultClassFactory<UserInterfaceLauncherFields, UserInterfaceLauncher_Cl, odRfl::DummyClass>;


    OD_DEFINE_CLASS(UserInterfaceLauncher, 0x008d, "System", "User Interface Launcher", UserInterfaceLauncherFactory);

}

#endif
