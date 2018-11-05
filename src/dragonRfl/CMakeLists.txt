

set(SOURCES
    "classes/common/ItemCommon.cpp"
    "classes/common/PlayerCommon.cpp"
    "classes/AnimationDemo.cpp"
    "classes/Building.cpp"
    "classes/CountDownToZero.cpp"
    "classes/Detector.cpp"
    "classes/DragonControl.cpp"
    "classes/DynamicLight.cpp"
    "classes/HealthPack.cpp"
    "classes/HumanControl.cpp"
    "classes/Key.cpp"
    "classes/Material.cpp"
    "classes/PushableObject.cpp"
    "classes/Sky.cpp"
    "classes/SoundEffect.cpp"
    "classes/StaticLight.cpp"
    "classes/Timer.cpp"
    "classes/TrackingCamera.cpp"
    "classes/UserInterfaceProperties.cpp"
    "classes/VisibilityToggler.cpp"
    "RflDragon.cpp")

add_library(dragonRfl STATIC ${SOURCES})

target_link_libraries(dragonRfl odCore ${OPENSCENEGRAPH_LIBRARIES} ${BULLET_LIBRARIES})
target_include_directories(dragonRfl PRIVATE ${OPENSCENEGRAPH_INCLUDE_DIRS} ${BULLET_INCLUDE_DIRS})
