#include "easybot_hardware.hpp"

#include <chrono>
#include <cmath>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "hardware_interface/types/hardware_interface_type_values.hpp"
#include "pluginlib/class_list_macros.hpp"
#include "rclcpp/rclcpp.hpp"

namespace easybot_hardware
{

/* ======================== on_init ======================== */
hardware_interface::CallbackReturn EasyBotHardware::on_init(
  const hardware_interface::HardwareInfo & info)
{
  if (hardware_interface::SystemInterface::on_init(info) !=
      hardware_interface::CallbackReturn::SUCCESS)
  {
    return hardware_interface::CallbackReturn::ERROR;
  }

  info_ = info;

  // 读取参数
  hw_slowdown_ = std::stod(info_.hardware_parameters["example_param_hw_slowdown"]);

  // 初始化 joint 数据
  hw_positions_.resize(info_.joints.size(), 0.0);
  hw_commands_.resize(info_.joints.size(), 0.0);

  RCLCPP_INFO(rclcpp::get_logger("EasyBotHardware"), "Hardware init successful");

  return hardware_interface::CallbackReturn::SUCCESS;
}

/* ======================== export_state_interfaces ======================== */
std::vector<hardware_interface::StateInterface>
EasyBotHardware::export_state_interfaces()
{
  std::vector<hardware_interface::StateInterface> state_interfaces;

  for (size_t i = 0; i < info_.joints.size(); i++)
  {
    state_interfaces.emplace_back(
      info_.joints[i].name,
      hardware_interface::HW_IF_POSITION,
      &hw_positions_[i]);
  }

  return state_interfaces;
}

/* ======================== export_command_interfaces ======================== */
std::vector<hardware_interface::CommandInterface>
EasyBotHardware::export_command_interfaces()
{
  std::vector<hardware_interface::CommandInterface> command_interfaces;

  for (size_t i = 0; i < info_.joints.size(); i++)
  {
    command_interfaces.emplace_back(
      info_.joints[i].name,
      hardware_interface::HW_IF_POSITION,
      &hw_commands_[i]);
  }

  return command_interfaces;
}

/* ======================== on_configure ======================== */
hardware_interface::CallbackReturn EasyBotHardware::on_configure(
  const rclcpp_lifecycle::State &)
{
  RCLCPP_INFO(rclcpp::get_logger("EasyBotHardware"), "Configuring...");


  // 重置状态
  for (size_t i = 0; i < hw_positions_.size(); i++)
  {
    hw_positions_[i] = 0.0;
    hw_commands_[i] = 0.0;
  }

  RCLCPP_INFO(rclcpp::get_logger("EasyBotHardware"), "Configured successfully");

  return hardware_interface::CallbackReturn::SUCCESS;
}

/* ======================== on_activate ======================== */
hardware_interface::CallbackReturn EasyBotHardware::on_activate(
  const rclcpp_lifecycle::State &)
{

  RCLCPP_INFO(rclcpp::get_logger("EasyBotHardware"), "Activating...");
  // 启动时 command = state
  for (size_t i = 0; i < hw_positions_.size(); i++)
  {
    hw_commands_[i] = hw_positions_[i];
  }
  RCLCPP_INFO(rclcpp::get_logger("EasyBotHardware"), "Activated successfully");

  return hardware_interface::CallbackReturn::SUCCESS;
}

/* ======================== on_deactivate ======================== */
hardware_interface::CallbackReturn EasyBotHardware::on_deactivate(
  const rclcpp_lifecycle::State &)
{
  
  RCLCPP_INFO(rclcpp::get_logger("EasyBotHardware"), "Deactivating...");
  RCLCPP_INFO(rclcpp::get_logger("EasyBotHardware"), "Deactivated successfully");

  return hardware_interface::CallbackReturn::SUCCESS;
}

/* ======================== read ======================== */
hardware_interface::return_type EasyBotHardware::read(
  const rclcpp::Time &, const rclcpp::Duration &)
{
  // 模拟关节运动（渐进到目标值）
  for (size_t i = 0; i < hw_positions_.size(); i++)
  {
    hw_positions_[i] += (hw_commands_[i] - hw_positions_[i]) / hw_slowdown_;
  }

  return hardware_interface::return_type::OK;
}

/* ======================== write ======================== */
hardware_interface::return_type EasyBotHardware::write(
  const rclcpp::Time &, const rclcpp::Duration &)
{

  return hardware_interface::return_type::OK;
}

}


/* ======================== 插件导出 ======================== */
PLUGINLIB_EXPORT_CLASS(
  easybot_hardware::EasyBotHardware,
  hardware_interface::SystemInterface)