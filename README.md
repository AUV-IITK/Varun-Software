[![Build Status](https://travis-ci.org/AUV-IITK/auv2016.svg?branch=master)](https://travis-ci.org/AUV-IITK/auv2016)

# [AUV Varun](http://auviitk.com/varun.html)

The main objective is to perform tasks like line detection, hitting a buoy,  and dropping markers underwater by using input data from sensors like IMU, monocular cameras, and a pressure sensor.  To decrease the complexity of the task, we broke it down into levels of abstraction with the help of ROS stacks.  

## Tasks 

We explain the tasks at hand in the following sub-sections

### Line Following

This is the first of the task that needs to be handled, the robot needs to follow the lines to guide through the arena and for this it should be able to come in the center of the line present beneath it as well as align itself with the line. This requires precise motion controls to stabilize and align the robot with the center of the line.

### Buoy Hitting
The task is to hit a circular buoy present at a definite height below the surface of water. The challenges involved are to localize the robot with respect to the center of the buoy using camera data and to move the robot steadily with rapidly fluctuating camera data. To make the system more robust we need to avoid the absurd motion due to noisy and sometimes false data obtained from camera due to improper detection and also need to handle the movement of the robot when the buoy is out of the camera frame.

### Passing through a Gate
The task is to pass the robot through an L-shaped gate without hitting its’ ends within a certain region as defined by the competition rules. The task was to locate the orientation(distance and angle) and the center of the gate from the robot’s position and then performing the required manoeuvre.

### Torpedo Shooting
The task at hand is to shoot the torpedo through a launcher when the robot is at a certain distance from two separately colored cupids such that it passes through the center of the targets.

## Implementation

The task at hand is gargantuan and needs to be abstracted out into different modules. ROS is perfect for helping out in this abstraction. For more information on different layers present in the code, refer to the following documentations:

* [__debug_layer__](debug_layer/README.md)
* [__master_layer__](master_layer/the_master/README.md): coordinates the tasks to be performed
* [__task_handler_layer__](task_handler_layer/README.md): implement individual tasks
* [__motion_library_layer__](motion_library_layer/README.md): handling individual calibrated motion
* [__hardware_layer__](hardware_layer/README.md)

## Contribution Guidelines

To get started with contributing to this repository, look out for open issues [here](https://github.com/AUV-IITK/auv2016/issues). Kindly read the [__Developer's Guide__](https://github.com/AUV-IITK/AUVWiki/wiki/Developers-Guide) before sending a pull request! :)
