
add_library(dragonRfl STATIC "")

set_target_properties(dragonRfl PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO)

target_sources(dragonRfl PRIVATE
    #"classes/common/ItemCommon.cpp"
    "classes/common/PlayerCommon.cpp"
    #"classes/AnimationDemo.cpp"
    "classes/Building.cpp"
    "classes/CountDownToZero.cpp"
    "classes/Detector.cpp"
    #"classes/DragonControl.cpp"
    "classes/DynamicLight.cpp"
    #"classes/HealthPack.cpp"
    "classes/HumanControl.cpp"
    "classes/HumanControlFields.cpp"
    #"classes/Key.cpp"
    #"classes/Material.cpp"
    #"classes/PushableObject.cpp"
    "classes/Sky.cpp"
    #"classes/SoundEffect.cpp"
    "classes/StaticLight.cpp"
    "classes/Timer.cpp"
    "classes/TrackingCamera.cpp"
    "classes/UserInterfaceProperties.cpp"
    #"classes/VisibilityToggler.cpp"
    "gui/CrystalRingButton.cpp"
    "gui/Cursor.cpp"
    "gui/DragonGui.cpp"
    "gui/GuiTextures.cpp"
    "gui/HealthIndicator.cpp"
    "gui/MainMenu.cpp"
    "Actions.cpp"
    "ClassRegistry.cpp"
    "Damage.cpp"
    "RflDragon.cpp")

target_link_libraries(dragonRfl odCore)
