
option(USE_FLUIDSYNTH "Whether to use FluidSynth subsystem to render music. (might be broken)" OFF)

add_executable(odOsg "")

set_target_properties(odOsg PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO)

target_sources(odOsg PRIVATE
    "audio/Buffer.cpp"
    "audio/OpenAlContext.cpp"
    "audio/SoundSystem.cpp"
    "audio/Source.cpp"
    "audio/StreamingSource.cpp"
    "audio/music/DummySynth.cpp"
    "audio/music/StupidSineSynth.cpp"
    "render/Camera.cpp"
    "render/Geometry.cpp"
    "render/Group.cpp"
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

target_link_libraries(odOsg odCore ${WHOLE_ARCHIVE_START} dragonRfl ${WHOLE_ARCHIVE_END})

find_package(OpenSceneGraph 3.4.0 REQUIRED osgDB osgViewer osgGA osgUtil)
target_link_libraries(odOsg ${OPENSCENEGRAPH_LIBRARIES})

find_package(OpenAL REQUIRED)
target_link_libraries(odOsg ${OPENAL_LIBRARY})

if(USE_FLUIDSYNTH)
    # FluidSynth is only available as a module via PkgConfig as of 2.0.6
    find_package(PkgConfig)
    if(PKGCONFIG_FOUND)
        pkg_check_modules(FLUID fluidsynth)
        if(FLUID_FOUND)
            target_link_libraries(odOsg ${FLUID_LIBRARIES})
            target_include_directories(odOsg PRIVATE ${FLUID_INCLUDE_DIRS})
            target_compile_definitions(odOsg PRIVATE USE_FLUIDSYNTH)
            target_sources(odOsg PRIVATE "audio/music/FluidSynth.cpp")
        else()
            message("No FluidSynth found. Disabling music.")
        endif()
    else()
        message("No pkgconfig found. pkgconfig is required to find FluidSynth. Disabling music.")
    endif()
endif()
