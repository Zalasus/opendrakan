
# config for odCore

set(SOURCES
        "anim/Animator.cpp"
        "anim/MotionAnimator.cpp"
        "anim/SkeletonAnimationPlayer.cpp"
        "audio/Buffer.cpp"
        "audio/SoundContext.cpp"
        "audio/SoundManager.cpp"
        "audio/Source.cpp"
        "db/Animation.cpp"
        "db/AnimationFactory.cpp"
        "db/Asset.cpp"
        "db/AssetProvider.cpp"
        "db/Class.cpp"
        "db/ClassFactory.cpp"
        "db/Database.cpp"
        "db/DbManager.cpp"
        "db/Model.cpp"
        "db/ModelFactory.cpp"
        "db/Sequence.cpp"
        "db/SequenceFactory.cpp"
        "db/Skeleton.cpp"
        "db/Sound.cpp"
        "db/SoundFactory.cpp"
        "db/Texture.cpp"
        "db/TextureFactory.cpp"
        "gui/ContainerWidget.cpp"
        "gui/GuiManager.cpp"
        "gui/TexturedQuad.cpp"
        "gui/Widget.cpp"
        "gui/WidgetGroup.cpp"
        "gui/WidgetIntersectVisitor.cpp"
        "render/Light.cpp"
        "render/RenderManager.cpp"
        "render/LightState.cpp"
        "physics/BulletCallbacks.cpp"
        "physics/CharacterController.cpp"
        "physics/DebugDrawer.cpp"
        "physics/Detector.cpp"
        "physics/ModelBounds.cpp"
        "physics/PhysicsManager.cpp"
        "rfl/ClassBuilderProbe.cpp"
        "rfl/Field.cpp"
        "rfl/FieldProbe.cpp"
        "rfl/ObjectBuilderProbe.cpp"
        "rfl/PrefetchProbe.cpp"
        "rfl/Rfl.cpp"
        "rfl/RflClass.cpp"
        "rfl/RflManager.cpp"
        "rfl/RflMessage.cpp"
        "DataStream.cpp"
        "Engine.cpp"
        "Exception.cpp"
        "FilePath.cpp"
        "GeodeBuilder.cpp"
        "InputManager.cpp"
        "Layer.cpp"
        "Level.cpp"
        "LevelObject.cpp"
        "Logger.cpp"
        "OsgSerializers.cpp"
        "ShaderManager.cpp"
        "SrscFile.cpp"
        "StringUtils.cpp"
        "UpdateCallback.cpp"
        "ZStream.cpp")
    
find_package(Threads REQUIRED)
    
add_library(odCore STATIC ${SOURCES})

target_link_libraries(odCore 
                        ${CMAKE_THREAD_LIBS_INIT}
                        ${OPENSCENEGRAPH_LIBRARIES} 
                        ${ZLIB_LIBRARIES} 
                        ${BULLET_LIBRARIES} 
                        ${OPENAL_LIBRARY})
                    
target_include_directories(odCore PRIVATE ${OPENSCENEGRAPH_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS} ${BULLET_INCLUDE_DIRS} ${OPENAL_INCLUDE_DIR})
    
    