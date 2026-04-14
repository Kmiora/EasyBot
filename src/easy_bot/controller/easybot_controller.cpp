#include "easybot_controller.hpp"
#include "pluginlib/class_list_macros.hpp"

namespace easybot_controller
{
EasyBotController::EasyBotController()
: controller_interface::ControllerInterface()
{}

controller_interface::CallbackReturn EasyBotController::on_init()
{
  try {
    // 检查是否已经声明过，避免 "already been declared" 错误
    if (!get_node()->has_parameter("joints")) {
      get_node()->declare_parameter("joints", std::vector<std::string>());
    }
    if (!get_node()->has_parameter("interface_name")) {
      get_node()->declare_parameter("interface_name", "");
    }
  } catch (const std::exception & e) {
    RCLCPP_ERROR(get_node()->get_logger(), "Init exception: %s", e.what());
    return controller_interface::CallbackReturn::ERROR;
  }
  return controller_interface::CallbackReturn::SUCCESS;
}

controller_interface::CallbackReturn EasyBotController::on_configure(const rclcpp_lifecycle::State &)
{
  joint_names_ = get_node()->get_parameter("joints").as_string_array();
  interface_name_ = get_node()->get_parameter("interface_name").as_string();

  if (joint_names_.empty() || interface_name_.empty()) {
    RCLCPP_ERROR(get_node()->get_logger(), "Joints or interface_name not set in YAML!");
    return controller_interface::CallbackReturn::ERROR;
  }

  // 订阅话题：这里的 "~/commands" 会自动展开为 "/控制器名/commands"
  joints_command_subscriber_ = get_node()->create_subscription<CmdType>(
    "~/commands", rclcpp::SystemDefaultsQoS(),
    [this](const CmdType::SharedPtr msg) { rt_command_ptr_.writeFromNonRT(msg); });

  return controller_interface::CallbackReturn::SUCCESS;
}

controller_interface::InterfaceConfiguration EasyBotController::command_interface_configuration() const
{
  controller_interface::InterfaceConfiguration config;
  config.type = controller_interface::interface_configuration_type::INDIVIDUAL;
  // 根据参数拼接接口全名，如 "joint1/position"
  for (const auto & joint : joint_names_) {
    config.names.push_back(joint + "/" + interface_name_);
  }
  return config;
}

controller_interface::InterfaceConfiguration EasyBotController::state_interface_configuration() const
{
  // 仅转发指令，不需要读取反馈，所以设为 NONE
  return controller_interface::InterfaceConfiguration{controller_interface::interface_configuration_type::NONE};
}

controller_interface::CallbackReturn EasyBotController::on_activate(const rclcpp_lifecycle::State &)
{
  rt_command_ptr_ = realtime_tools::RealtimeBuffer<std::shared_ptr<CmdType>>(nullptr);
  return controller_interface::CallbackReturn::SUCCESS;
}

controller_interface::CallbackReturn EasyBotController::on_deactivate(const rclcpp_lifecycle::State &)
{
  rt_command_ptr_ = realtime_tools::RealtimeBuffer<std::shared_ptr<CmdType>>(nullptr);
  return controller_interface::CallbackReturn::SUCCESS;
}

controller_interface::return_type EasyBotController::update(const rclcpp::Time &, const rclcpp::Duration &)
{
  auto joint_commands = rt_command_ptr_.readFromRT();

  // 判空检查
  if (!joint_commands || !(*joint_commands)) return controller_interface::return_type::OK;

  // 数据长度校验
  if ((*joint_commands)->data.size() != command_interfaces_.size()) {
    return controller_interface::return_type::ERROR;
  }

  // 写入硬件
  for (auto i = 0u; i < command_interfaces_.size(); ++i) {
    command_interfaces_[i].set_value((*joint_commands)->data[i]);
  }

  return controller_interface::return_type::OK;
}

}

// 插件导出，必须位于命名空间外
PLUGINLIB_EXPORT_CLASS(easybot_controller::EasyBotController, controller_interface::ControllerInterface)