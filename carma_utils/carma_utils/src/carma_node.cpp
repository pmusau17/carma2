// Copyright 2021 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "carma_utils/carma_node.hpp"

#include <memory>
#include <string>
#include <vector>

#include "lifecycle_msgs/msg/state.hpp"
#include "rclcpp_components/register_node_macro.hpp"

namespace carma_utils
{

CarmaNode::CarmaNode(const rclcpp::NodeOptions & options)
: rclcpp_lifecycle::LifecycleNode("carma_node", "", options)
{
  // TODO(@pmusau17): Creation of pubs and sub should be in on_configure

  // Create a system alert publisher. The subscriber will be made by the child class
  system_alert_pub_ = create_publisher<cav_msgs::msg::SystemAlert>(system_alert_topic_, 10);

  RCLCPP_INFO(get_logger(), "Lifecycle node launched, waiting on state transition requests");
}

CarmaNode::~CarmaNode()
{
  RCLCPP_INFO(get_logger(), "Destroying");

  // In case this lifecycle node wasn't properly shut down, do it here
  if (get_current_state().id() == lifecycle_msgs::msg::State::PRIMARY_STATE_ACTIVE) {
    on_deactivate(get_current_state());
    on_cleanup(get_current_state());
  }
}

std::shared_ptr<carma_utils::CarmaNode>
CarmaNode::shared_from_this()
{
  return std::static_pointer_cast<carma_utils::CarmaNode>(
    rclcpp_lifecycle::LifecycleNode::shared_from_this());
}

carma_utils::CallbackReturn
CarmaNode::on_configure(const rclcpp_lifecycle::State & /*state*/)
{
  return carma_utils::CallbackReturn::SUCCESS;
}

carma_utils::CallbackReturn
CarmaNode::on_activate(const rclcpp_lifecycle::State & /*state*/)
{
  return carma_utils::CallbackReturn::SUCCESS;
}

carma_utils::CallbackReturn
CarmaNode::on_deactivate(const rclcpp_lifecycle::State & /*state*/)
{
  return carma_utils::CallbackReturn::SUCCESS;
}

carma_utils::CallbackReturn
CarmaNode::on_cleanup(const rclcpp_lifecycle::State & /*state*/)
{
  return carma_utils::CallbackReturn::SUCCESS;
}

void
CarmaNode::publish_system_alert(const cav_msgs::msg::SystemAlert::SharedPtr msg)
{
  system_alert_pub_->publish(*msg);
}

void
CarmaNode::on_system_alert(const cav_msgs::msg::SystemAlert::SharedPtr msg)
{
  RCLCPP_INFO(get_logger(), "Received SystemAlert message of type: %u", msg->type);
}

void
CarmaNode::spin()
{
  try {
    rclcpp::spin(get_node_base_interface());
  } catch (const std::exception & e) {
    RCLCPP_ERROR(
      get_logger(),
      "Handle exception %s, and issue system alert as you wish", e.what());
  }
}

void
CarmaNode::create_rclcpp_node(const rclcpp::NodeOptions & options)
{
  std::vector<std::string> new_args = options.arguments();
  new_args.push_back("--ros-args");
  new_args.push_back("-r");
  new_args.push_back(std::string("__node:=") + get_name() + "_rclcpp_node");
  new_args.push_back("--");
  rclcpp_node_ = std::make_shared<rclcpp::Node>(
    "_", get_namespace(), rclcpp::NodeOptions(options).arguments(new_args));
  rclcpp_thread_ = std::make_unique<ros2_utils::NodeThread>(rclcpp_node_);
}

}  // namespace carma_utils

// Register the component with class_loader
RCLCPP_COMPONENTS_REGISTER_NODE(carma_utils::CarmaNode)