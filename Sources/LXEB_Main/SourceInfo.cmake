CollectSources("${PROJECT_MODULE_ROOT}" ModSources)

add_executable(LXEB ${ModSources})
target_link_libraries(LXEB PRIVATE LXEB-api)