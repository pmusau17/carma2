# Build and run the CARMA2 source code

## Create a colcon workspace

```
export CARMA_WS=~/src/carma2_ws
mkdir -p $CARMA_WS/src
cd $CARMA_WS/src
```

## Download the CARMA2 source code

```
git clone https://github.com/mjeronimo/carma2
vcs import < carma2/carma2.repos
rosdep install -r --from-paths . --ignore-src --rosdistro $ROS_DISTRO -y
```

## Build CARMA2

```
cd $CARMA_WS
colcon build --symlink-install
```

## Source the CARMA2 workspace

```
source $CARMA_WS/install/setup.bash
```

## Launch the system

```
ros2 launch carma_bringup carma_launch.py
```

# Build and run CARMA2 in a Docker container

## Build the Docker image

```
cd $CARMA_WS/src/carma2/docker
./build-image.sh
```

## Launch the Docker container

```
docker run -it openrobotics/carma2:master
```

# Features/Capabilities

The example code in this repository provides the following features:

* CMake code
    * Can be included by CARMA packages
    * The carma_project() macro serves as a single source for compiler settings, etc.

* CarmaNode
    * A base class for CARMA nodes in the system
    * Derives from rclcpp_lifecycle::LifecycleNode (could also be a template that can use either Node or LifecycleNode)
    * Automatically creates a bond with the lifecycle manager (see: https://github.com/ros/bond_core)
    * System alert capable

* Perception Subsystem
    * All nodes are lifecycle nodes
    * All nodes are composable nodes loaded into a single container
        * A stub Carma Delphi Srr2 Driver
        * A stub Carma Velodyne LiDAR Driver
        * A sample "camera driver" sending images to a client
        * A sample camera client to receive images
    * No-copy messages communication within the subsystem

* Localization Subsystem
    * All nodes are lifecycle nodes
        * A stub dead_reckoner node
        * A stub ekf_localizer node node
        * Localization Health Monitor
            * With sample parameters
            * Handles LocalizationStatusReport messages
            * Messages sent manually using a sample command line program

* System Controller
    * A Lifecycle Manager that manages the state of the CARMA nodes (lifecycle nodes), initiating state transitions
    * Monitors bonds with the managed nodes, can detect lack of heartbeat and initialize restarted nodes
        * The launch system restarts the nodes, based on "respawn" setting
    * Messages (such as SHUTDOWN) can be sent manually via a command-line program
    * System alert capable

# Architecture Questions

* What is the recovery strategy?
    * Notify the system monitor?
    * Monitor and reset a subsystem?

* How does Launch interact with Lifecycle nodes and recovery?
    * Which component owns restarting the nodes?

* If a ComposableNode crashes, does it bring down the container?

# ROS 2 Porting considerations

* Launch
* Lifecycle nodes
* Parameters
* Subsystems
* System Eventing and state machines
* Composable nodes (especially in subsystems)
* Recovery

# Issues

* When setting the composable node container to respawn, the contained composable nodes aren't reloaded

# Notes

    Localization system recovery
        LiDAR-based localization fails, transitions output pose to GPS
        Localization Manager
            Monitoring the performance of the system and heartbeat status
            Implements recovery internally

        Separate node from the health monitoring
            Monitor and then decide when to shut down
            Checking heartbeat status of the nodes

    Activate:
	ros2 service call /carma_system_controller/manage_nodes ros2_lifecycle_manager_msgs/ManageLifecycleNodes "{ command: 1 }"

    Deactivate:
	ros2 service call /carma_system_controller/manage_nodes ros2_lifecycle_manager_msgs/ManageLifecycleNodes "{ command: 2 }"

# Task List

```
[x] Sample usage of the rclcpp node, such as using a message filter
[x] Sample usage from a CARMA node of a transform listener (doesn't need the rclcpp_node)
[ ] Using a helper class that itself creates pubs/subs, but accepts a node (not itself a node)
[ ] Using a plugin (a kind of helper; not a node itself, but takes a node to use)
[ ] Review all usages of timers. Create timers in on_configure and then deactivate and reactivate them,
    without using our own member variables (like "active_"). ROS 2 timers support cancel() and reset().
    Can use these methods in on_activate (reset) and on_deactivate(cancel)? See out the ROS 2 example at
    demo_nodes_cpp/src/timers/reuse_timer.cpp.
[ ] Make CarmaNode a template that can accept either rclcpp::Node or rclcpp_lifecycle::LifecycleNode
```
