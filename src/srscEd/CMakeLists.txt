

add_executable(srsced "")

set_target_properties(srsced PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO)

target_sources(srsced PRIVATE "Main.cpp")

target_link_libraries(srsced odCore)
