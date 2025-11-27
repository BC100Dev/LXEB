CollectSources("${PROJECT_MODULE_ROOT}" ModSources)

add_library(LXEB-api SHARED "${ModSources}")