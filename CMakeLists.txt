file(GLOB CM_PRECONF_FILES "${CMAKE_CURRENT_SOURCE_DIR}/CM_Config/PreConfig/*.cmake")
foreach (CM_PRECONF_FILE ${CM_PRECONF_FILES})
    message(STATUS "Running PreConfig file ${CM_PRECONF_FILE}")
    include("${CM_PRECONF_FILE}")
endforeach ()

set(CM_PROJECT_NAME LXEB)

# top-level check
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    cmake_minimum_required(VERSION 3.25)
    project(${CM_PROJECT_NAME})
elseif (NOT PROJECT_NAME)
    project(${CM_PROJECT_NAME})
endif ()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CROSSCOMPILING ON)

file(GLOB CM_CONFIG_FILES "${CMAKE_CURRENT_SOURCE_DIR}/CM_Config/*.cmake")
foreach (CM_CONFIG_FILE ${CM_CONFIG_FILES})
    include("${CM_CONFIG_FILE}")
endforeach ()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Include")

file(GLOB Private_CM_Modules "${CMAKE_CURRENT_SOURCE_DIR}/Sources/*")
foreach (Private_CM_Module ${Private_CM_Modules})
    if (EXISTS "${Private_CM_Module}/SourceInfo.cmake")
        set(PROJECT_MODULE_ROOT "${Private_CM_Module}")
        include("${PROJECT_MODULE_ROOT}/SourceInfo.cmake")

        message(STATUS "Project Index Successful: ${PROJECT_MODULE_ROOT}")
    else ()
        message(STATUS "Project Index Ignore: ${Private_CM_Module}")
    endif ()
endforeach ()

file(GLOB CM_POSTCONF_FILES "${CMAKE_CURRENT_SOURCE_DIR}/CM_Config/PostConfig/*.cmake")
foreach (CM_POSTCONF_FILE ${CM_POSTCONF_FILES})
    message(STATUS "Running PostConfig file ${CM_POSTCONF_FILE}")
    include("${CM_POSTCONF_FILE}")
endforeach ()