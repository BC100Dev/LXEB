CollectSources("${PROJECT_MODULE_ROOT}/src" ModSources)

add_library(LXEB-package SHARED ${ModSources})
target_link_libraries(LXEB-package PRIVATE ${APP_LINK_DEPS} LXEB-api LXEB-shared)
target_include_directories(LXEB-package PUBLIC "${PROJECT_MODULE_ROOT}/include")