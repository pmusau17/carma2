cmake_minimum_required(VERSION 3.5)
project(camera_driver)

find_package(ament_cmake REQUIRED)
find_package(carma_common REQUIRED)
find_package(carma_utils REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_lifecycle REQUIRED)
find_package(ros2_lifecycle_manager_msgs REQUIRED)
find_package(image_transport REQUIRED)
find_package(OpenCV REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(rcutils REQUIRED)

carma_package()

include_directories(
  include
)

set(executable_name ${PROJECT_NAME})
set(library_name ${executable_name}_core)

set(dependencies
  carma_common
  carma_utils
  image_transport
  OpenCV
  rclcpp
  rclcpp_components
  rclcpp_lifecycle
  rcutils
  ros2_lifecycle_manager_msgs
  sensor_msgs
)

set(node_plugins "")

add_library(${library_name} SHARED
  src/camera_driver.cpp
)

target_compile_definitions(${library_name}
  PRIVATE "COMPOSITION_BUILDING_DLL"
)

ament_target_dependencies(${library_name}
  ${dependencies}
)

rclcpp_components_register_nodes(${library_name}
  "camera_driver::CameraDriver"
)

set(node_plugins "${node_plugins}camera_driver::CameraDriver;$<TARGET_FILE:camera_driver_core>\n")

add_executable(${executable_name}
  src/main.cpp
)

target_link_libraries(${executable_name} ${library_name})

install(TARGETS ${library_name}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(DIRECTORY include/
  DESTINATION include/
)

install(DIRECTORY resources
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_export_include_directories(include)
ament_export_libraries(${library_name})
ament_export_dependencies(${dependencies})
ament_package()
