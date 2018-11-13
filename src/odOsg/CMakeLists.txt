
set(SOURCES
        "Main.cpp"
        "ObjectHandle.cpp"
        "Renderer.cpp"
        "ShaderFactory.cpp")
        
find_package(OpenSceneGraph 3.4.0 REQUIRED osgDB osgViewer osgGA osgUtil)

add_executable(odOsg ${SOURCES})

target_link_libraries(odOsg 
                        odCore 
                        ${WHOLE_ARCHIVE_START} dragonRfl ${WHOLE_ARCHIVE_END}
                        ${OPENSCENEGRAPH_LIBRARIES})
