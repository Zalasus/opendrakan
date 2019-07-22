

add_executable(srsced "")

target_sources(srsced PRIVATE "Main.cpp")

target_link_libraries(srsced odCore)