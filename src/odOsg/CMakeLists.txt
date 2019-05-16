
set(SOURCES
        "audio/Buffer.cpp"
        "audio/OpenAlContext.cpp"
        "audio/SoundSystem.cpp"
        "audio/Source.cpp"
        "render/Camera.cpp"
        "render/Geometry.cpp"
        "render/GeometryBuilder.cpp"
        "render/GuiNode.cpp"
        "render/GuiQuad.cpp"
        "render/Handle.cpp"
        "render/Image.cpp"
        "render/LayerNode.cpp"
        "render/LightState.cpp"
        "render/Model.cpp"
        "render/ModelNode.cpp"
        "render/ObjectNode.cpp"
        "render/PhysicsDebugDrawer.cpp"
        "render/Renderer.cpp"
        "render/Rig.cpp"
        "render/ShaderFactory.cpp"
        "render/Texture.cpp"
        "InputListener.cpp"
        "Main.cpp")
        
find_package(OpenSceneGraph 3.4.0 REQUIRED osgDB osgViewer osgGA osgUtil)
find_package(OpenAL REQUIRED)

add_executable(odOsg ${SOURCES})

target_link_libraries(odOsg 
                        odCore 
                        ${WHOLE_ARCHIVE_START} dragonRfl ${WHOLE_ARCHIVE_END}
                        ${OPENSCENEGRAPH_LIBRARIES}
                        ${OPENAL_LIBRARY})
