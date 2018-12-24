

set(SOURCES
        "Main.cpp")
        
add_executable(srsced ${SOURCES})

target_link_libraries(srsced 
                        odCore)