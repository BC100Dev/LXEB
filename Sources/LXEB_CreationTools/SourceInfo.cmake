## This module will end up being used to perform various tasks, which include:
## - Creating LXEB packages (./LXEB-Creator package <outfile>)
## - Extracting LXEB packages (./LXEB-Creator extract <arch> <dir>)
## - Patching LXEB packages (./LXEB-Creator patch <infile> <patched_manifest> <patch_data>... <outfile>)
## - Validating, whether the LXEB package can be fully loaded successfully (./LXEB-Creator test-load <infile>)
## - Test-running a test / debug build of LXEB with the given package (./LXEB-Creator test-run <infile> <web_server>)
#### This requires a valid web server for the needed web server provider (e.g. Moodle) to work
## - Signing LXEB packages (./LXEB-Creator sign <infile> <cert|public_cert> <ca_file|self> <outfile>)

string(TOLOWER ${CMAKE_BUILD_TYPE} bt_type)
if (bt_type STREQUAL "release")
    message(STATUS "Ignoring the CreatorTools for a release build")
    return()
endif ()

CollectSources("${PROJECT_MODULE_ROOT}" CreatorSources)
add_executable(LXEB-Creator ${CreatorSources})
target_link_libraries(LXEB-Creator PRIVATE ${APP_LINK_DEPS} LXEB-shared LXEB-api)