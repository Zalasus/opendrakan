

set(SOURCES
    "common/ItemCommon.cpp"
    "common/PlayerCommon.cpp"
    "gui/CrystalRingButton.cpp"
    "gui/Cursor.cpp"
    "gui/HealthIndicator.cpp"
    "gui/LoadingBar.cpp"
    "gui/MainMenu.cpp"
    "AnimationDemo.cpp"
    "Building.cpp"
    "CountDownToZero.cpp"
    "Detector.cpp"
    "DragonControl.cpp"
    "DynamicLight.cpp"
    "HealthPack.cpp"
    "HumanControl.cpp"
    "Key.cpp"
    "Material.cpp"
    "PushableObject.cpp"
    "RflDragon.cpp"
    "Sky.cpp"
    "StaticLight.cpp"
    "Timer.cpp"
    "TrackingCamera.cpp"
    "UserInterfaceProperties.cpp"
    "VisibilityToggler.cpp")

# FIXME: since making a static library here would cause the self-registering classes to break, we have to make it shared for now

add_library(dragonRfl SHARED ${SOURCES})

target_link_libraries(dragonRfl odCore ${OPENSCENEGRAPH_LIBRARIES} ${BULLET_LIBRARIES})
target_include_directories(dragonRfl PRIVATE ${OPENSCENEGRAPH_INCLUDE_DIRS} ${BULLET_INCLUDE_DIRS})
