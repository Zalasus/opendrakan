

cmake_minimum_required(VERSION 3.0.0)

project(OpenDrakan CXX)

option(BUILD_SRCSED "Build srscEd, a viewer for SRSC-files (useful for reverse-engineering)" ON)
option(BUILD_CLASSSTAT "Build classStat, a tool for dumping .odb class data" ON)
option(BUILD_OSG_RENDERER "Build the OpenSceneGraph-based renderer" ON)

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

# determine the flag we have to use to link a whole archive (used for linking RFLs)
if((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
    set(WHOLE_ARCHIVE_START "-Wl,--whole-archive")
    set(WHOLE_ARCHIVE_END   "-Wl,--no-whole-archive")
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(WHOLE_ARCHIVE_START "/WHOLEARCHIVE")
    set(WHOLE_ARCHIVE_END   "")
endif()

# make sure we get all binaries in the top level binary directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# version header generation
set(VERSION_HEADER_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include)
set(VERSION_HEADER_FILE ${VERSION_HEADER_INCLUDE_DIR}/odCore/Version.h)
find_package(Git)
if(Git_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Version.h.in ${CMAKE_CURRENT_BINARY_DIR}/Version.h.in COPYONLY)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/VersionCache.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/VersionCache.cmake.in COPYONLY)
    add_custom_target(GenerateVersion ALL
                      COMMAND ${CMAKE_COMMAND} -DGIT_EXECUTABLE=${GIT_EXECUTABLE} -P ${CMAKE_CURRENT_SOURCE_DIR}/GenerateVersion.cmake
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
else()
    message(STATUS "Git not found. Generating empty version header")
    set(VERSION_COMMIT n/a)
    set(VERSION_TAG n/a)
    set(VERSION_BRANCH n/a)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Version.h.in ${VERSION_HEADER_FILE})
endif()

include_directories("include")

add_subdirectory("src/odCore")
add_subdirectory("src/dragonRfl")

if(BUILD_OSG_RENDERER)
    add_subdirectory("src/odOsg")
endif()

if(BUILD_SRSCED)
    add_subdirectory("src/srscEd")
endif()

if(BUILD_CLASSSTAT)
    add_subdirectory("src/classStat")
endif()

# copy shader sources
set(SHADER_SOURCES
        "resources/shader_src/model_vertex.glsl"
        "resources/shader_src/model_fragment.glsl"
        "resources/shader_src/layer_vertex.glsl"
        "resources/shader_src/layer_fragment.glsl"
        "resources/shader_src/default_vertex.glsl"
        "resources/shader_src/default_fragment.glsl")

foreach(f ${SHADER_SOURCES})
    # configure file creates dependencies between source and target
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${f} ${CMAKE_CURRENT_BINARY_DIR}/${f} COPYONLY)
endforeach()
