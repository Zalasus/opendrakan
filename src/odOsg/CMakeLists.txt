
set(SOURCES
        "Geometry.cpp"
        "GeometryBuilder.cpp"
        "Main.cpp"
        "ModelNode.cpp"
        "ObjectNode.cpp"
        "Renderer.cpp"
        "ShaderFactory.cpp"
        "Texture.cpp")
        
find_package(OpenSceneGraph 3.4.0 REQUIRED osgDB osgViewer osgGA osgUtil)

add_executable(odOsg ${SOURCES})

target_link_libraries(odOsg 
                        odCore 
                        ${WHOLE_ARCHIVE_START} dragonRfl ${WHOLE_ARCHIVE_END}
                        ${OPENSCENEGRAPH_LIBRARIES})
