
add_executable(classStat ${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp)

set_target_properties(classStat PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO)

target_link_libraries(classStat odCore)
