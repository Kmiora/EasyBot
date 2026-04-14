cmake_minimum_required(VERSION 3.8)
project(easy_bot)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ===================== dependencies =====================
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_lifecycle REQUIRED)

find_package(hardware_interface REQUIRED)
find_package(controller_interface REQUIRED)
find_package(pluginlib REQUIRED)
find_package(realtime_tools REQUIRED)
find_package(std_msgs REQUIRED)

# ===================== hardware =====================
add_library(easy_bot_hardware SHARED
  hardware/easy_bot.cpp
)

target_include_directories(easy_bot_hardware PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/hardware/include>
  $<INSTALL_INTERFACE:include>
)

ament_target_dependencies(easy_bot_hardware
  hardware_interface
  pluginlib
  rclcpp
  rclcpp_lifecycle
)


# ===================== controller =====================
add_library(easy_bot_controller SHARED
  controller/easy_bot_controller.cpp
)

target_include_directories(easy_bot_controller PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/controller/include>
  $<INSTALL_INTERFACE:include>
)

ament_target_dependencies(easy_bot_controller
  controller_interface
  rclcpp
  realtime_tools
  std_msgs
)


# ===================== plugin export =====================
pluginlib_export_plugin_description_file(
  hardware_interface hardware_interface.xml
)

pluginlib_export_plugin_description_file(
  controller_interface controller_interface.xml
)

# ===================== install =====================
install(DIRECTORY urdf launch config
  DESTINATION share/${PROJECT_NAME}
)

install(TARGETS
  easy_bot_hardware
  EXPORT export_${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(TARGETS
  easy_bot_controller
  EXPORT export_${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(DIRECTORY hardware/include controller/include
  DESTINATION include
)

ament_export_dependencies(
  rclcpp
  rclcpp_lifecycle
  hardware_interface
  controller_interface
  pluginlib
  realtime_tools
  std_msgs
)

ament_package()