
#ifndef INCLUDE_RFL_DRAGON_USERINTERFACELAUNCHER_H_
#define INCLUDE_RFL_DRAGON_USERINTERFACELAUNCHER_H_

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

        odRfl::EnumImpl<UiTypeEnum, 0, 2> launchWhat;
    };


    class UserInterfaceLauncher_Sv final : public odRfl::ServerClass, public odRfl::SpawnableClass, public odRfl::ClassImpl<UserInterfaceLauncher_Sv>
    {
    public:

        virtual odRfl::FieldBundle &getFields() override { return mFields; }

        virtual void onMessageReceived() override { bounce(); }


    private:

        UserInterfaceLauncherFields mFields;

    };


    class UserInterfaceLauncher_Cl final : public odRfl::ClientClass, public odRfl::SpawnableClass, public odRfl::ClassImpl<UserInterfaceLauncher_Cl>
    {
    public:

        virtual void onRemoteMessageReceived() override;


    private:

        UserInterfaceLauncherFields mFields;

    };


    OD_DEFINE_CLASS(UserInterfaceLauncher, 0x008d, "System", "User Interface Launcher", UserInterfaceLauncherFactory);

}

#endif
