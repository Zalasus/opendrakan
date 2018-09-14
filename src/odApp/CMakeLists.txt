

set(SOURCES
        "Main.cpp")
        

add_executable(opendrakan ${SOURCES})

target_link_libraries(opendrakan odCore dragonRfl ${OPENSCENEGRAPH_LIBRARIES} ${ZLIB_LIBRARIES} ${BULLET_LIBRARIES})
target_include_directories(opendrakan PRIVATE ${OPENSCENEGRAPH_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS} ${BULLET_INCLUDE_DIRS})

# make sure to output the binary in the root of the build directory
set_target_properties(opendrakan PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})