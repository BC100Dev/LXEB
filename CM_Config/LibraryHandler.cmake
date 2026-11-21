find_package(Qt6 REQUIRED COMPONENTS
        Core
        Gui
        Widgets
        Network
        WebEngineWidgets
        WaylandCompositor
        WaylandClient)
find_package(OpenSSL REQUIRED)

# import from OsintgramCXX
set(CURL_USE_OPENSSL ON)
set(CURL_USE_SCHANNEL OFF)

set(CMAKE_NO_SYSTEM_FROM_IMPORTED ON)

## Encryption / Decryption relevant tasks (required by CURL for HTTPS communication)
find_package(OpenSSL REQUIRED)

## fucking vcpkg
if (NOT TARGET CURL::win32_winsock)
    add_library(CURL::win32_winsock INTERFACE IMPORTED)
    set_target_properties(CURL::win32_winsock PROPERTIES
            INTERFACE_LINK_LIBRARIES "ws2_32"
    )
endif ()

## Required for Networking
find_package(CURL REQUIRED)

## Required for Local Account Storage (used by CURL)
find_package(ZLIB REQUIRED)

## Let's make it a requirement, especially for Windows users
find_package(Threads REQUIRED)

list(APPEND APP_LINK_DEPS CURL::libcurl ZLIB::ZLIB)
list(APPEND APP_LINK_OpenSSL OpenSSL::SSL OpenSSL::Crypto)

if (DEFINED APP_TARGETS_ANDROID AND NOT CMAKE_SYSTEM_NAME STREQUAL "Android")
    include(UseJava)
endif ()

if (APP_SYSTEM_TARGET STREQUAL "Windows" OR CMAKE_SYSTEM_NAME STREQUAL "Windows" OR WIN32 OR MINGW)
    list(APPEND APP_LINK_DEPS crypt32 ws2_32 advapi32 secur32)
    set(CMAKE_SYSTEM_IGNORE_PATH
            /usr/include
            /usr/local/include
    )
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

    ## issues with MinGW on Arch: re-create the libraries for OpenSSL that it needs
    # why? stupid "-ldl" flag.
    # hot garbage, am I right or am I right
    if ("dl" IN_LIST OPENSSL_LIBRARIES)
        list(REMOVE_ITEM OPENSSL_LIBRARIES "dl")
    endif ()

    set_property(TARGET OpenSSL::Crypto PROPERTY INTERFACE_LINK_LIBRARIES "crypt32;ws2_32")
    set_property(TARGET OpenSSL::SSL PROPERTY INTERFACE_LINK_LIBRARIES "crypt32;ws2_32")

    list(APPEND APP_LINK_DEPS
            "-no-pthread"
            "-Wl,--undefined=clock_gettime64"
            "-Wl,--undefined=nanosleep64"
            "-l:libwinpthread.a")

    if (APP_SYSTEM_TARGET STREQUAL "Windows")
        set(CMAKE_SHARED_LIBRARY_PREFIX "")
        set(CMAKE_SHARED_LIBRARY_SUFFIX ".dll")
        set(CMAKE_STATIC_LIBRARY_PREFIX "")
        set(CMAKE_STATIC_LIBRARY_SUFFIX ".lib")
        set(CMAKE_EXECUTABLE_SUFFIX ".exe")
    endif ()
endif ()

if (APP_TARGETS_ANDROID OR CMAKE_SYSTEM_NAME STREQUAL "Android")
    include(UseJava)
    include(FindJava)
endif ()