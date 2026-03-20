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

function(SetTargetOutputDir TARGET DIR)
    set_target_properties(${TARGET} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${DIR}"
            LIBRARY_OUTPUT_DIRECTORY "${DIR}"
            ARCHIVE_OUTPUT_DIRECTORY "${DIR}"
    )
endfunction()

function(SetRpathValue TARGET)
    set(RPATH_VAL "$ORIGIN:$ORIGIN/libs:$ORIGIN/lib:$ORIGIN/../lib:$ORIGIN/../libs")
    set_target_properties(${TARGET} PROPERTIES
            BUILD_WITH_INSTALL_RPATH TRUE
            INSTALL_RPATH "${RPATH_VAL}"
            BUILD_RPATH "${RPATH_VAL}"
            INSTALL_RPATH_USE_LINK_PATH FALSE)
endfunction()