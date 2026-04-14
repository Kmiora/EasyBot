import launch
import launch_ros
from ament_index_python.packages import get_package_share_directory
from launch.substitutions import Command, LaunchConfiguration
from launch_ros.parameter_descriptions import ParameterValue
import os

def generate_launch_description():
    urdf_package_path=get_package_share_directory('easy_bot')
    default_urdf_path=os.path.join(urdf_package_path,'urdf','easy_bot.urdf.xacro')
    default_rviz_path=os.path.join(urdf_package_path,'config','rviz_config.rviz')

    robot_description = ParameterValue(
        Command(['xacro ', default_urdf_path]),
        value_type=str
    )

    action_robot_state_publisher=launch_ros.actions.Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        parameters=[{'robot_description':robot_description}]
    )

    action_joint_state_publisher=launch_ros.actions.Node(
        package='joint_state_publisher',
        executable='joint_state_publisher',
    )

    action_rviz_node=launch_ros.actions.Node(
        package='rviz2',
        executable='rviz2',
        arguments=['-d',default_rviz_path]
    )

    return launch.LaunchDescription([
        action_robot_state_publisher,
        action_joint_state_publisher,
        action_rviz_node
    ])