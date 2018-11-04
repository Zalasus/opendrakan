
# config for odCore

set(SOURCES
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
        "physics/BulletCallbacks.cpp"
        "physics/CharacterController.cpp"
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
        "Layer.cpp"
        "Level.cpp"
        "LevelObject.cpp"
        "Logger.cpp"
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
                    
target_include_directories(odCore PRIVATE ${ZLIB_INCLUDE_DIRS} ${BULLET_INCLUDE_DIRS})
    
    