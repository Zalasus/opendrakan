

cmake_minimum_required(VERSION 2.6.3)

project(OpenDrakan CXX)

set(CMAKE_CXX_STANDARD 14)

if(NOT CMAKE_BUILD_TYPE)
    message("No CMAKE_BUILD_TYPE specified. Defaulting to Debug")
    set(CMAKE_BUILD_TYPE Debug)
endif()

# enable compiler warnings when generating debug makefiles
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    endif()
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()
endif()


# dependencies
find_package(ZLIB REQUIRED)
find_package(OpenSceneGraph 3.2.0 REQUIRED osgDB osgViewer osgGA osgUtil)
find_package(Bullet 2.8.3 REQUIRED Collision Dynamics LinearMath)

include_directories("include")

add_subdirectory("src/odCore")
add_subdirectory("src/dragonRfl")
add_subdirectory("src/odApp")


# copy shader sources
set(SHADER_SOURCES
        "resources/shader_src/default_vertex.glsl"
        "resources/shader_src/default_fragment.glsl"
        "resources/shader_src/rigged_vertex.glsl"
        "resources/shader_src/crystal_fragment.glsl")

foreach(f ${SHADER_SOURCES})
    # configure file creates dependencies between source and target
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${f} ${CMAKE_CURRENT_BINARY_DIR}/${f} COPYONLY)
endforeach()

