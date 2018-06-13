

cmake_minimum_required(VERSION 2.6.3)

project(OpenDrakan CXX)

set (CMAKE_CXX_STANDARD 14)

if(NOT CMAKE_BUILD_TYPE)
    message("No CMAKE_BUILD_TYPE specified. Defaulting to Debug")
    set(CMAKE_BUILD_TYPE Debug)
endif()

# project sources
include_directories("include")
set(SOURCES
        "src/StringUtils.cpp"
        "src/db/AssetProvider.cpp"
        "src/db/Animation.cpp"
        "src/db/AnimationFactory.cpp"
        "src/db/Skeleton.cpp"
        "src/db/Asset.cpp"
        "src/db/Sound.cpp"
        "src/db/Class.cpp"
        "src/db/ClassFactory.cpp"
        "src/db/TextureFactory.cpp"
        "src/db/Model.cpp"
        "src/db/Texture.cpp"
        "src/db/Sequence.cpp"
        "src/db/ModelFactory.cpp"
        "src/db/Database.cpp"
        "src/db/SequenceFactory.cpp"
        "src/db/SoundFactory.cpp"
        "src/physics/ModelBounds.cpp"
        "src/physics/PhysicsManager.cpp"
        "src/physics/DebugDrawer.cpp"
        "src/physics/CharacterController.cpp"
        "src/physics/BulletCallbacks.cpp"
        "src/SrscFile.cpp"
        "src/rfl/RflClass.cpp"
        "src/rfl/Rfl.cpp"
        "src/rfl/RflFieldProbe.cpp"
        "src/rfl/RflField.cpp"
        "src/rfl/RflMessage.cpp"
        "src/rfl/PrefetchProbe.cpp"
        "src/rfl/common/ItemCommon.cpp"
        "src/rfl/common/PlayerCommon.cpp"
        "src/rfl/dragon/RflDragon.cpp"
        "src/rfl/dragon/Material.cpp"
        "src/rfl/dragon/TrackingCamera.cpp"
        "src/rfl/dragon/Sky.cpp"
        "src/rfl/dragon/Timer.cpp"
        "src/rfl/dragon/HealthPack.cpp"
        "src/rfl/dragon/Key.cpp"
        "src/rfl/dragon/HumanControl.cpp"
        "src/rfl/dragon/DragonControl.cpp"
        "src/rfl/dragon/Building.cpp"
        "src/rfl/dragon/PushableObject.cpp"
        "src/rfl/dragon/CountDownToZero.cpp"
        "src/rfl/dragon/UserInterfaceProperties.cpp"
        "src/anim/Animator.cpp"
        "src/anim/MotionAnimator.cpp"
        "src/anim/SkeletonAnimationPlayer.cpp"
        "src/gui/GuiManager.cpp"
        "src/Layer.cpp"
        "src/OsgSerializers.cpp"
        "src/DbManager.cpp"
        "src/ZStream.cpp"
        "src/Engine.cpp"
        "src/DataStream.cpp"
        "src/SegmentedGeode.cpp"
        "src/TextureAtlas.cpp"
        "src/Logger.cpp"
        "src/InputManager.cpp"
        "src/Level.cpp"
        "src/Exception.cpp"
        "src/LevelObject.cpp"
        "src/FilePath.cpp"
        "src/ShaderManager.cpp"
        "src/GeodeBuilder.cpp"
        "src/Main.cpp")


# dependencies
find_package(ZLIB REQUIRED)
find_package(OpenSceneGraph 3.2.0 REQUIRED osgDB osgViewer osgGA osgUtil)
find_package(Bullet 2.8.3 REQUIRED Collision Dynamics LinearMath)


# targets
add_executable(opendrakan ${SOURCES})
target_link_libraries(opendrakan ${OPENSCENEGRAPH_LIBRARIES} ${ZLIB_LIBRARIES} ${BULLET_LIBRARIES})
target_include_directories(opendrakan PRIVATE ${OPENSCENEGRAPH_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS} ${BULLET_INCLUDE_DIRS})


# copy shader sources
set(SHADER_SOURCES
        "shader_src/default_vertex.glsl"
        "shader_src/default_fragment.glsl"
        "shader_src/rigged_vertex.glsl")

foreach(f ${SHADER_SOURCES})
    # configure file creates dependencies between source and target
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${f} ${CMAKE_CURRENT_BINARY_DIR}/${f} COPYONLY)
endforeach()
