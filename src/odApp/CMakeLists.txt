

set(SOURCES
        "Main.cpp")
        
find_package(glfw3 REQUIRED)
        
add_executable(opendrakan ${SOURCES})

target_link_libraries(opendrakan 
                        odCore 
                        ${WHOLE_ARCHIVE_START} dragonRfl ${WHOLE_ARCHIVE_END}
                        ${GLFW3_LIBRARIES})
