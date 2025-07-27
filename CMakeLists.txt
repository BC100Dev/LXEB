cmake_minimum_required(VERSION 3.28)
project(LXEB)

set(CMAKE_CXX_STANDARD 20)

file(GLOB CONFIG_FILES "${CMAKE_CURRENT_SOURCE_DIR}/CM_Config/*.cmake")
foreach (CONFIG_FILE ${CONFIG_FILES})
    include("${CONFIG_FILE}")
endforeach ()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Include")

file(GLOB APP_INCL_SourceInfo_PRIV_LOOKUP "${CMAKE_CURRENT_SOURCE_DIR}/Sources/*")
foreach (APP_INCL_SourceInfo_PRIV_FILE ${APP_INCL_SourceInfo_PRIV_LOOKUP})
    if (EXISTS "${APP_INCL_SourceInfo_PRIV_FILE}/SourceInfo.cmake")
        set(PROJECT_MODULE_ROOT "${APP_INCL_SourceInfo_PRIV_FILE}")
        include("${APP_INCL_SourceInfo_PRIV_FILE}/SourceInfo.cmake")

        message(STATUS "Indexed Module on Path ${APP_INCL_SourceInfo_PRIV_FILE}")
    endif ()
endforeach ()
