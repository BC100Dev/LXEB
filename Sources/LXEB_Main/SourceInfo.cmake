CollectSources("${PROJECT_MODULE_ROOT}" ModSources)

add_executable(LXEB ${ModSources})
set_target_properties(LXEB PROPERTIES AUTOMOC ON)
target_link_libraries(LXEB PRIVATE LXEB-api
        Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network
        Qt6::WebEngineWidgets Qt6::WaylandCompositor Qt6::WaylandClient)