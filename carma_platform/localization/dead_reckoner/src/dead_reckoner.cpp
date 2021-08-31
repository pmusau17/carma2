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

#include "dead_reckoner/dead_reckoner.hpp"

#include <memory>
#include <string>
#include <vector>

#include "dead_reckoner/distance_calculator.hpp"
#include "pluginlib/class_loader.hpp"

namespace dead_reckoner
{

DeadReckoner::DeadReckoner(const rclcpp::NodeOptions & options)
: CarmaLifecycleNode(options)
{
  create_rclcpp_node(options);
}

carma_utils::CallbackReturn
DeadReckoner::on_configure(const rclcpp_lifecycle::State & state)
{
  RCLCPP_INFO(get_logger(), "Configuring");
  CarmaLifecycleNode::on_configure(state);

  try {
    dc = distance_loader.createSharedInstance("dead_reckoner::DistanceCalculator");
    dc->initialize(shared_from_this());
    dc->configure();
  } catch (pluginlib::PluginlibException & ex) {
    RCLCPP_INFO(get_logger(), "The plugin failed to load for some reason. Error: %s\n", ex.what());
  }

  // Initialize transform listener and broadcaster
  tf_buffer_ = std::make_shared<tf2_ros::Buffer>(rclcpp_node_->get_clock());
  auto timer_interface = std::make_shared<tf2_ros::CreateTimerROS>(
    rclcpp_node_->get_node_base_interface(),
    rclcpp_node_->get_node_timers_interface());
  tf_buffer_->setCreateTimerInterface(timer_interface);
  tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);
  tf_broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>(rclcpp_node_);

  // Initialize the message filter
  image_sub_ = std::make_unique<message_filters::Subscriber<sensor_msgs::msg::Image>>(
    rclcpp_node_.get(), "camera/image", rmw_qos_profile_sensor_data);

  image_filter_ = std::make_unique<tf2_ros::MessageFilter<sensor_msgs::msg::Image>>(
    *image_sub_, *tf_buffer_, "camera", 10, rclcpp_node_, tf2::durationFromSec(0.0));

  image_connection_ = image_filter_->registerCallback(
    std::bind(
      &DeadReckoner::on_image_received,
      this, std::placeholders::_1));

  return carma_utils::CallbackReturn::SUCCESS;
}

carma_utils::CallbackReturn
DeadReckoner::on_activate(const rclcpp_lifecycle::State & state)
{
  RCLCPP_INFO(get_logger(), "Activating");
  CarmaLifecycleNode::on_activate(state);
  dc->activate();

  return carma_utils::CallbackReturn::SUCCESS;
}

carma_utils::CallbackReturn
DeadReckoner::on_deactivate(const rclcpp_lifecycle::State & state)
{
  RCLCPP_INFO(get_logger(), "Deactivating");
  CarmaLifecycleNode::on_deactivate(state);
  dc->deactivate();
  return carma_utils::CallbackReturn::SUCCESS;
}

carma_utils::CallbackReturn
DeadReckoner::on_cleanup(const rclcpp_lifecycle::State & state)
{
  RCLCPP_INFO(get_logger(), "Cleaning up");
  CarmaLifecycleNode::on_cleanup(state);

  // Image filter
  image_connection_.disconnect();
  image_filter_.reset();
  image_sub_.reset();

  // Transforms
  tf_broadcaster_.reset();
  tf_listener_.reset();
  tf_buffer_.reset();
  dc->cleanup();
  return carma_utils::CallbackReturn::SUCCESS;
}

carma_utils::CallbackReturn
DeadReckoner::on_shutdown(const rclcpp_lifecycle::State & /*state*/)
{
  RCLCPP_INFO(get_logger(), "Shutting down");
  return carma_utils::CallbackReturn::SUCCESS;
}

carma_utils::CallbackReturn
DeadReckoner::on_error(const rclcpp_lifecycle::State & /*state*/)
{
  RCLCPP_FATAL(get_logger(), "Lifecycle node error");
  return carma_utils::CallbackReturn::SUCCESS;
}

void
DeadReckoner::on_system_alert(const cav_msgs::msg::SystemAlert::SharedPtr msg)
{
  RCLCPP_INFO(
    get_logger(), "Received SystemAlert message of type: %u, msg: %s",
    msg->type, msg->description.c_str());
  RCLCPP_INFO(get_logger(), "Perform DeadReckoner-specific system event handling");
}

void
DeadReckoner::on_image_received(sensor_msgs::msg::Image::ConstSharedPtr image)
{
  RCLCPP_INFO(
    get_logger(), "Image received in frame: %s", image->header.frame_id.c_str());
}

}  // namespace dead_reckoner
