

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

include_directories("include")

add_subdirectory("src/odCore")
add_subdirectory("src/dragonRfl")
add_subdirectory("src/odOsg")
add_subdirectory("src/srscEd")

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

