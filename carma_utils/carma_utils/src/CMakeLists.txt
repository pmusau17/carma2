add_library(${library_name} SHARED
  carma_node.cpp
  carma_lifecycle_node.cpp
)

set(node_plugins "")
target_compile_definitions(${library_name}
  PRIVATE "COMPOSITION_BUILDING_DLL")
rclcpp_components_register_nodes(${library_name} "carma_utils::CarmaNode")
set(node_plugins "${node_plugins}carma_utils::CarmaNode;$<TARGET_FILE:carma_utils_core>\n")

add_executable(event_pub
  publish_events.cpp
)

ament_target_dependencies(event_pub
  ${dependencies}
)

ament_target_dependencies(${library_name}
  ${dependencies}
)

install(TARGETS ${library_name} event_pub
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)
