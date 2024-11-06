CollectSources(${MODULE_ROOT} MODULE_SOURCE_FILES)

add_executable(LXEB_Client ${MODULE_SOURCE_FILES})
target_link_libraries(LXEB_Client PRIVATE ${APP_GLOBAL_LinkList})