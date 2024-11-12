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

    set(${OutVar} ${ALL_FILES} PARENT_SCOPE)
endfunction()