

set(SOURCES
        "Main.cpp")
        
add_executable(opendrakan ${SOURCES})

target_link_libraries(opendrakan 
                        odCore 
                        ${WHOLE_ARCHIVE_START} dragonRfl ${WHOLE_ARCHIVE_END})
