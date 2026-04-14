#ifndef EASY_BOT__EASY_BOT_CONTROLLER_HPP_
#define EASY_BOT__EASY_BOT_CONTROLLER_HPP_

#include <memory>
#include <string>
#include <vector>

#include "controller_interface/controller_interface.hpp"
#include "realtime_tools/realtime_buffer.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"

namespace easy_bot_controller
{
using CmdType = std_msgs::msg::Float64MultiArray;

class EasyBotController : public controller_interface::ControllerInterface
{
public:
  EasyBotController();

  // 这里的配置决定了控制器如何与硬件接口“握手”
  controller_interface::InterfaceConfiguration command_interface_configuration() const override;
  controller_interface::InterfaceConfiguration state_interface_configuration() const override;

  // 状态机生命周期函数
  controller_interface::CallbackReturn on_init() override;
  controller_interface::CallbackReturn on_configure(const rclcpp_lifecycle::State & previous_state) override;
  controller_interface::CallbackReturn on_activate(const rclcpp_lifecycle::State & previous_state) override;
  controller_interface::CallbackReturn on_deactivate(const rclcpp_lifecycle::State & previous_state) override;

  // 核心控制循环
  controller_interface::return_type update(const rclcpp::Time & time, const rclcpp::Duration & period) override;

protected:
  std::vector<std::string> joint_names_;
  std::string interface_name_;
  
  // 实时缓冲区：非实时线程（订阅者回调）往里写，实时线程（update）从中读
  realtime_tools::RealtimeBuffer<std::shared_ptr<CmdType>> rt_command_ptr_;
  rclcpp::Subscription<CmdType>::SharedPtr joints_command_subscriber_;
};

}  // namespace easy_bot

#endif  // EASY_BOT__EASY_BOT_CONTROLLER_HPP_