cmake_minimum_required(VERSION 3.5)
project(system_controller)


find_package(ros2_lifecycle_manager REQUIRED)
find_package(ament_cmake REQUIRED)
find_package(carma_common REQUIRED)
find_package(carma_utils REQUIRED)
find_package(cav_msgs REQUIRED)
find_package(lifecycle_msgs REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(rclcpp_lifecycle REQUIRED)
find_package(rclcpp REQUIRED)
find_package(ros2_lifecycle_manager_msgs REQUIRED)
find_package(ros2_utils REQUIRED)
find_package(std_srvs REQUIRED)

carma_package()

include_directories(
  include
)

set(executable_name ${PROJECT_NAME})
set(library_name ${executable_name}_core)

set(dependencies
  carma_common
  carma_utils
  rclcpp
  rclcpp_lifecycle
  ros2_lifecycle_manager
  ros2_lifecycle_manager_msgs
)

add_library(${library_name} SHARED
  src/system_controller.cpp
)

ament_target_dependencies(${library_name}
  ${dependencies}
)

add_executable(${executable_name}
  src/main.cpp
)

target_link_libraries(${executable_name} ${library_name})

ament_target_dependencies(${executable_name}
  ${dependencies}
)

install(TARGETS ${library_name}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(TARGETS ${executable_name}
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY include/
  DESTINATION include/
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_export_include_directories(include)
ament_export_libraries(${library_name})
ament_export_dependencies(${dependencies})
ament_package()