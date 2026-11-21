## Disabling executables on DevTools for this one
## This ends up being used across most of the project over here

CollectSources("${PROJECT_MODULE_ROOT}/redist" CommonsSources)
CollectSources("${PROJECT_MODULE_ROOT}/net" NetSources)
#CollectSources("${PROJECT_MODULE_ROOT}/executables" ExecutableSources)

add_library(LXEB-shared SHARED ${CommonsSources} ${NetSources})
target_link_libraries(LXEB-shared PUBLIC ${APP_LINK_DEPS})
SetRpathValue(LXEB-shared)

if (APP_SYSTEM_TARGET STREQUAL "Windows")
    target_link_libraries(LXEB-shared PUBLIC ole32 oleaut32 shell32 uuid wbemuuid)
endif ()