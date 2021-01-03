

# script to generate version header from git info

set(VERSION_CACHE_FILE VersionCache.cmake)
set(VERSION_CACHE_FILE_PROTO VersionCache.cmake.in)
set(VERSION_HEADER_FILE include/odCore/Version.h)
set(VERSION_HEADER_FILE_PROTO Version.h.in)


# when running a cmake script in a custom command, we can't access the regular cache.
#  we make do by writing the version info into a second script in the build dir

if(EXISTS ${VERSION_CACHE_FILE})
    include(${VERSION_CACHE_FILE})
else()
    set(VERSION_TAG    "")
    set(VERSION_BRANCH "")
    set(VERSION_COMMIT "")
endif()

message(STATUS "Running git to fetch current version info")
execute_process(COMMAND ${GIT_EXECUTABLE} describe
                OUTPUT_VARIABLE VERSION_TAG_NEW
                RESULT_VARIABLE GIT_RESULT
                OUTPUT_STRIP_TRAILING_WHITESPACE)
if(NOT GIT_RESULT EQUAL 0)
    set(VERSION_TAG_NEW "n/a")
endif()

execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
                OUTPUT_VARIABLE VERSION_BRANCH_NEW
                RESULT_VARIABLE GIT_RESULT
                OUTPUT_STRIP_TRAILING_WHITESPACE)
if(NOT GIT_RESULT EQUAL 0)
    set(VERSION_BRANCH_NEW "n/a")
endif()

execute_process(COMMAND ${GIT_EXECUTABLE} log -n1 --pretty=%h
                OUTPUT_VARIABLE VERSION_COMMIT_NEW
                RESULT_VARIABLE GIT_RESULT
                OUTPUT_STRIP_TRAILING_WHITESPACE)
if(NOT GIT_RESULT EQUAL 0)
    set(VERSION_COMMIT_NEW "n/a")
endif()

if((VERSION_TAG_NEW STREQUAL VERSION_TAG) AND (VERSION_BRANCH_NEW STREQUAL VERSION_BRANCH) AND (VERSION_COMMIT_NEW STREQUAL VERSION_COMMIT))

    message(STATUS "Version info has not changed. Not regenerating version header")

else()

    message(STATUS "Version info has changed: ${VERSION_TAG_NEW} ${VERSION_BRANCH_NEW} ${VERSION_COMMIT_NEW}")

    configure_file(${VERSION_CACHE_FILE_PROTO} ${VERSION_CACHE_FILE})

    set(VERSION_TAG    ${VERSION_TAG_NEW})
    set(VERSION_BRANCH ${VERSION_BRANCH_NEW})
    set(VERSION_COMMIT ${VERSION_COMMIT_NEW})

    configure_file(${VERSION_HEADER_FILE_PROTO} ${VERSION_HEADER_FILE})

endif()
