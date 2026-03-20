CollectSources(${SEB_Submodule} ModSources)

add_library(SEB-monitoring SHARED ${ModSources})
target_link_libraries(SEB-monitoring PRIVATE SEB-integrity SEB-logging SEB-systemComponents)