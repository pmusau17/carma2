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

#ifndef DEAD_RECKONER__DEAD_RECKONER_HPP_
#define DEAD_RECKONER__DEAD_RECKONER_HPP_

#include <memory>
#include "tf2_ros/buffer.h"
#include "tf2_ros/message_filter.h"
#include "tf2_ros/transform_broadcaster.h"
#include "tf2_ros/transform_listener.h"
#include "tf2_ros/create_timer_ros.h"
#include "message_filters/subscriber.h"
#include "carma_utils/carma_node.hpp"
#include "carma_utils/visibility_control.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "rclcpp/rclcpp.hpp"


namespace dead_reckoner
{

class DeadReckoner : public carma_utils::CarmaNode
{
public:
  CARMA_UTILS_PUBLIC
  explicit DeadReckoner(const rclcpp::NodeOptions & options);

protected:
  carma_utils::CallbackReturn on_configure(const rclcpp_lifecycle::State & state) override;
  carma_utils::CallbackReturn on_activate(const rclcpp_lifecycle::State & state) override;
  carma_utils::CallbackReturn on_deactivate(const rclcpp_lifecycle::State & state) override;
  carma_utils::CallbackReturn on_cleanup(const rclcpp_lifecycle::State & state) override;
  carma_utils::CallbackReturn on_shutdown(const rclcpp_lifecycle::State & state) override;
  carma_utils::CallbackReturn on_error(const rclcpp_lifecycle::State & state) override;


  // transfrom parameters
  std::shared_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
  std::shared_ptr<tf2_ros::TransformListener> tf_listener_;
  std::shared_ptr<tf2_ros::Buffer> tf_buffer_;

  // message filters
  std::unique_ptr<message_filters::Subscriber<sensor_msgs::msg::Image>> image_sub_;
  std::unique_ptr<tf2_ros::MessageFilter<sensor_msgs::msg::Image>> image_filter_;
  message_filters::Connection image_connection_;

  void initMessageFilters();
  void imageReceived(sensor_msgs::msg::Image::ConstSharedPtr image);
  void on_system_alert(const cav_msgs::msg::SystemAlert::SharedPtr msg);
};

}  // namespace dead_reckoner

#endif  //  DEAD_RECKONER__DEAD_RECKONER_HPP_
