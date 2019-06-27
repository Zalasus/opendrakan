
set(SOURCES
        "audio/Buffer.cpp"
        "audio/OpenAlContext.cpp"
        "audio/SoundSystem.cpp"
        "audio/Source.cpp"
        "audio/StreamingSource.cpp"
        "audio/music/FluidSynth.cpp"
        "render/Camera.cpp"
        "render/Geometry.cpp"
        "render/GuiNode.cpp"
        "render/GuiQuad.cpp"
        "render/Handle.cpp"
        "render/Image.cpp"
        "render/LightState.cpp"
        "render/Model.cpp"
        "render/ModelBuilder.cpp"
        "render/PhysicsDebugDrawer.cpp"
        "render/Renderer.cpp"
        "render/Rig.cpp"
        "render/ShaderFactory.cpp"
        "render/Texture.cpp"
        "InputListener.cpp"
        "Main.cpp")
        
find_package(OpenSceneGraph 3.4.0 REQUIRED osgDB osgViewer osgGA osgUtil)
find_package(OpenAL REQUIRED)
find_package(fluidsynth REQUIRED)

add_executable(odOsg ${SOURCES})

target_link_libraries(odOsg 
                        odCore 
                        ${WHOLE_ARCHIVE_START} dragonRfl ${WHOLE_ARCHIVE_END}
                        ${OPENSCENEGRAPH_LIBRARIES}
                        ${OPENAL_LIBRARY}
                        ${FLUIDSYNTH_LIBRARY})
