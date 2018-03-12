

cmake_minimum_required(VERSION 2.6.3)

project(OpenDrakan)


# project sources TODO: list files instead of globbing
include_directories("include")
file(GLOB_RECURSE SOURCES "src/*.cpp" "test_src/*.cpp")


# dependencies
find_package(ZLIB REQUIRED)
find_package(OpenSceneGraph 3.4.0 REQUIRED osgDB osgViewer osgGA osgUtil)

include_directories(${ZLIB_INCLUDE_DIRS})
include_directories(${OPENSCENEGRAPH_INCLUDE_DIRS})


# targets
add_executable(opendrakan ${SOURCES})
target_link_libraries(opendrakan ${OPENSCENEGRAPH_LIBRARIES} ${ZLIB_LIBRARIES})