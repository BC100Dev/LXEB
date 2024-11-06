function(CollectSources RootDir OutVar)
    set(SEARCH_PATTERNS
            "${RootDir}/*.cpp"
            "${RootDir}/**/*.cpp"
            "${RootDir}/*.c"
            "${RootDir}/**/*.c"
            "${RootDir}/*.hpp"
            "${RootDir}/**/*.hpp"
            "${RootDir}/*.h"
            "${RootDir}/**/*.h"
    )

    file(GLOB_RECURSE ALL_FILES ${SEARCH_PATTERNS})
    set(SOURCES "")

    foreach (FILE ${ALL_FILES})
        if (NOT FILE MATCHES "${CMAKE_SOURCE_DIR}/build" AND NOT FILE MATCHES "${CMAKE_BINARY_DIR}")
            list(APPEND SOURCES ${FILE})
        endif ()
    endforeach ()

    set(${OutVar} ${SOURCES} PARENT_SCOPE)
endfunction()