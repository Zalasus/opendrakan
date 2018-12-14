
# config for odCore

set(SOURCES
        "anim/Skeleton.cpp"
        "anim/SkeletonAnimationPlayer.cpp"
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
        "db/SkeletonBuilder.cpp"
        "db/Sound.cpp"
        "db/SoundFactory.cpp"
        "db/Texture.cpp"
        "db/TextureFactory.cpp"
        "gui/ContainerWidget.cpp"
        "gui/DrawableWidget.cpp"
        "gui/GuiManager.cpp"
        "gui/Widget.cpp"
        "physics/BulletCallbacks.cpp"
        "physics/CharacterController.cpp"
        "physics/Detector.cpp"
        "physics/ModelBounds.cpp"
        "physics/PhysicsManager.cpp"
        "render/Light.cpp"
        "rfl/ClassBuilderProbe.cpp"
        "rfl/Field.cpp"
        "rfl/FieldProbe.cpp"
        "rfl/ObjectBuilderProbe.cpp"
        "rfl/PrefetchProbe.cpp"
        "rfl/Rfl.cpp"
        "rfl/RflClass.cpp"
        "rfl/RflManager.cpp"
        "rfl/RflMessage.cpp"
        "BoundingBox.cpp"
        "BoundingSphere.cpp"
        "DataStream.cpp"
        "Engine.cpp"
        "Exception.cpp"
        "FilePath.cpp"
        "Layer.cpp"
        "Level.cpp"
        "LevelObject.cpp"
        "NuLogger.cpp"
        "RefCounted.cpp"
        "GlmSerializers.cpp"
        "SrscFile.cpp"
        "StringUtils.cpp"
        "ZStream.cpp")
    
find_package(Threads REQUIRED)
    
add_library(odCore STATIC ${SOURCES})

target_link_libraries(odCore 
                        ${CMAKE_THREAD_LIBS_INIT}
                        ${ZLIB_LIBRARIES} 
                        ${BULLET_LIBRARIES})
                    
target_include_directories(odCore PRIVATE ${ZLIB_INCLUDE_DIRS})
target_include_directories(odCore PUBLIC ${GLM_INCLUDE_DIRS} ${BULLET_INCLUDE_DIRS})
    
    