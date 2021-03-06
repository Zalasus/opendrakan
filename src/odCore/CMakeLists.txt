
# config for odCore

add_library(odCore STATIC "")

set_target_properties(odCore PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO)

target_sources(odCore PRIVATE
        "anim/SequencePlayer.cpp"
        "anim/Skeleton.cpp"
        "anim/SkeletonAnimationPlayer.cpp"
        "audio/music/SegmentPlayer.cpp"
        "audio/SoundSystem.cpp"
        "db/Animation.cpp"
        "db/Asset.cpp"
        "db/AssetRef.cpp"
        "db/Class.cpp"
        "db/ClassFactory.cpp"
        "db/Database.cpp"
        "db/DbManager.cpp"
        "db/DependencyTable.cpp"
        "db/Model.cpp"
        "db/ModelBounds.cpp"
        "db/MusicContainer.cpp"
        "db/Segment.cpp"
        "db/Sequence.cpp"
        "db/SkeletonDefinition.cpp"
        "db/Sound.cpp"
        "db/Texture.cpp"
        "db/TextureFactory.cpp"
        "gui/Gui.cpp"
        "gui/Quad.cpp"
        "gui/Widget.cpp"
        "input/Action.cpp"
        "input/InputListener.cpp"
        "input/InputManager.cpp"
        "net/MessageDispatcher.cpp"
        "net/LocalTunnel.cpp"
        "net/PacketBuilder.cpp"
        "net/PacketParser.cpp"
        "net/QueuedDownlinkConnector.cpp"
        "net/QueuedUplinkConnector.cpp"
        "physics/bullet/BulletCallbacks.cpp"
        "physics/bullet/BulletPhysicsSystem.cpp"
        "physics/bullet/DebugDrawer.cpp"
        "physics/bullet/LayerHandleImpl.cpp"
        "physics/bullet/LightHandleImpl.cpp"
        "physics/bullet/ManagedCompoundShape.cpp"
        "physics/bullet/ModelShapeImpl.cpp"
        "physics/bullet/ObjectHandleImpl.cpp"
        "physics/CharacterController.cpp"
        "physics/Handles.cpp"
        "physics/PhysicsSystem.cpp"
        "render/Renderer.cpp"
        "rfl/ClassBuilderProbe.cpp"
        #"rfl/DefaultObjectClass.cpp"
        "rfl/Field.cpp"
        "rfl/FieldLoaderProbe.cpp"
        "rfl/FieldProbe.cpp"
        "rfl/ObjectBuilderProbe.cpp"
        "rfl/PrefetchProbe.cpp"
        "rfl/Rfl.cpp"
        "rfl/Class.cpp"
        "rfl/RflManager.cpp"
        "state/Event.cpp"
        "state/EventQueue.cpp"
        "state/StateBundleDetail.cpp"
        "state/StateManager.cpp"
        "BoundingBox.cpp"
        "BoundingSphere.cpp"
        "Client.cpp"
        "ConfigFile.cpp"
        "DataStream.cpp"
        "Engine.cpp"
        "FilePath.cpp"
        "Guid.cpp"
        "Layer.cpp"
        "Level.cpp"
        "LevelObject.cpp"
        "Light.cpp"
        "Message.cpp"
        "NuLogger.cpp"
        "ObjectLightReceiver.cpp"
        "ObjectRecord.cpp"
        "Panic.cpp"
        "RiffReader.cpp"
        "GlmSerializers.cpp"
        "Server.cpp"
        "SrscFile.cpp"
        "StringUtils.cpp"
        "ThreadUtils.cpp"
        "ZStream.cpp")

add_dependencies(odCore GenerateVersion)
target_include_directories(odCore PUBLIC ${VERSION_HEADER_INCLUDE_DIR})

find_package(Threads REQUIRED)
target_link_libraries(odCore ${CMAKE_THREAD_LIBS_INIT})
if(CMAKE_USE_PTHREADS_INIT)
    target_compile_definitions(odCore PUBLIC USE_PTHREADS)
endif()

find_package(ZLIB REQUIRED)
target_link_libraries(odCore ${ZLIB_LIBRARIES})
target_include_directories(odCore PRIVATE ${ZLIB_INCLUDE_DIRS})

find_package(Bullet 2.8.3 REQUIRED Collision LinearMath)
target_link_libraries(odCore ${BULLET_LIBRARIES})
target_include_directories(odCore PRIVATE ${BULLET_INCLUDE_DIRS})

# FIXME: making this required breaks the travis build on ubuntu xenial. for now we make it optional and display a warning if not found
find_package(glm 0.9.8)
if(glm_FOUND)
    target_link_libraries(odCore PUBLIC ${GLM_LIBRARY})
    target_include_directories(odCore PUBLIC ${GLM_INCLUDE_DIRS})
    # since some glm versions (not necessarily older ones) require this:
    target_compile_definitions(odCore PUBLIC GLM_ENABLE_EXPERIMENTAL)
else()
    message(WARNING "Could not find glm cmake package. Trying to continue generating, as build works anyway on most systems if it is installed")
endif()
