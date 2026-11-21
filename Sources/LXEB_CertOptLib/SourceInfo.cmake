string(TOLOWER "${CMAKE_BUILD_TYPE}" bt_type)
CollectSources("${PROJECT_MODULE_ROOT}/src" ModSources)

add_library(LXEB-certoptlib STATIC ${ModSources})
target_link_libraries(LXEB-certoptlib PUBLIC ${APP_LINK_DEPS})
target_include_directories(LXEB-certoptlib PUBLIC "${PROJECT_MODULE_ROOT}/include")

if (bt_type STREQUAL "release")
    target_compile_definitions(LXEB-certoptlib PUBLIC HAS_PUBLIC_BUILD=1)
else ()
    target_compile_definitions(LXEB-certoptlib PUBLIC HAS_PUBLIC_BUILD=0)
endif ()