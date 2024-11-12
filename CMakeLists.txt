cmake_minimum_required(VERSION 3.28)
project(LXEB)

set(CMAKE_CXX_STANDARD 20)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Functions.cmake")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Include")

set(APP_GLOBAL_LinkList "")

find_package(Qt6 REQUIRED COMPONENTS Core Widgets WebEngineWidgets)
list(APPEND APP_GLOBAL_LinkList Qt6::Core Qt6::Widgets Qt6::WebEngineWidgets)

file(GLOB APP_INCL_SourceInfo_PRIV_LOOKUP "${CMAKE_CURRENT_SOURCE_DIR}/Sources/*")
set(APP_MODULES_INDEXED "")
foreach (APP_INCL_SourceInfo_PRIV_FILE ${APP_INCL_SourceInfo_PRIV_LOOKUP})
    if (EXISTS "${APP_INCL_SourceInfo_PRIV_FILE}/SourceInfo.cmake")
        set(MODULE_ROOT "${APP_INCL_SourceInfo_PRIV_FILE}")
        include("${APP_INCL_SourceInfo_PRIV_FILE}/SourceInfo.cmake")

        message(STATUS "Indexed Module on Path ${APP_INCL_SourceInfo_PRIV_FILE}")
    endif ()
endforeach ()

set(MODULE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/Installer")
include("${CMAKE_CURRENT_SOURCE_DIR}/Installer/SourceInfo.cmake")
