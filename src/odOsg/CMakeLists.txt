

set(SOURCES
        "Main.cpp"
        "Renderer.cpp")
    
    
find_package(OpenSceneGraph 3.4.0 REQUIRED osgDB osgViewer osgGA osgUtil)


add_executable(odOsg ${SOURCES})


if((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
    set(WHOLE_ARCHIVE_START "-Wl,--whole-archive")
    set(WHOLE_ARCHIVE_END   "-Wl,--no-whole-archive")
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(WHOLE_ARCHIVE_START "/WHOLEARCHIVE")
    set(WHOLE_ARCHIVE_END   "")
endif()

target_link_libraries(opendrakan 
                        odCore 
                        ${WHOLE_ARCHIVE_START} dragonRfl ${WHOLE_ARCHIVE_END}
                        ${OSG_LIBRARIES})
