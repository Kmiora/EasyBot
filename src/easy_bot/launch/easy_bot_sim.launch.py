import launch
import launch_ros
from ament_index_python.packages import get_package_share_directory
from launch.substitutions import Command, LaunchConfiguration
from launch_ros.parameter_descriptions import ParameterValue
import os

def generate_launch_description():
    urdf_package_path=get_package_share_directory('easy_bot')
    default_xacro_path=os.path.join(urdf_package_path,'urdf','easy_bot.urdf.xacro')
    default_rviz_path=os.path.join(urdf_package_path,'config','rviz_config.rviz')


    robot_description = ParameterValue(
        Command(['xacro ', default_xacro_path]),
        value_type=str
    )


    action_robot_state_publisher=launch_ros.actions.Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        parameters=[{'robot_description':robot_description}]
    )

    action_control_node = launch_ros.actions.Node(
        package='controller_manager',
        executable='ros2_control_node',
        parameters=[
            {'robot_description': robot_description},
            os.path.join(urdf_package_path, 'config', 'easybot_controller.yaml')
        ],
        output='screen'
    )

    joint_state_broadcaster_spawner = launch_ros.actions.Node(
        package='controller_manager',
        executable='spawner',
        arguments=['robot_joint_state_broadcaster'],
    )

    # forward_controller_spawner = launch_ros.actions.Node(
    #     package='controller_manager',
    #     executable='spawner',
    #     arguments=['forward_command_controller'],
    # )

    easybot_controller_spawner = launch_ros.actions.Node(
        package='controller_manager',
        executable='spawner',
        arguments=['easybot_controller'],
    )

    action_rviz_node=launch_ros.actions.Node(
        package='rviz2',
        executable='rviz2',
        arguments=['-d',default_rviz_path]
    )

    return launch.LaunchDescription([
        action_robot_state_publisher,
        action_control_node,
        joint_state_broadcaster_spawner,
        easybot_controller_spawner,
        action_rviz_node
    ])