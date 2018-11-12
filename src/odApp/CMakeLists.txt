

set(SOURCES
        "Main.cpp")
        
find_package(glfw3 REQUIRED)
        
add_executable(opendrakan ${SOURCES})

# we need to link the whole RFL so the registrars work 
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
                        ${GLFW3_LIBRARIES})
