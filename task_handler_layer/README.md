# Task Handler Layer

## Task Buoy 

This package is to make the robot hit the buoy. It will take the goal from master layer.

* Wait until goal is received from the master layer.
* Wait for motion library server to start.
* Start stabilization of turning motion by sending a goal of 0 angle to turn.
* Send a goal to upward motion library to make the buoy at vertical center of the camera of robot. The data for current position is sent by Image Processing node of object detection.
* Once robot is in line of buoy, it starts upward stabilization in similar manner and
sends a goal to sideward motion library to make it in the horizontal center of the
robot. The present position data is sent by Image Processing node to the motion
library.
* At this point, the robot and the buoy are in the same line. It sends a goal to
forward motion library to move the robot forward until it hits the buoy
* Stop stabilization and forward motion

## Task Gate 

This package is to make the robot pass the gate. It will take the goal from master layer.

* Wait until goal is received from the master layer.
* Wait for motion library server to start.
* Send a goal to upward motion library to make the gate at vertical center of the camera of robot. The data for current position is sent by Image Processing node of gate detection.
* Start stabilization of turning motion by sending a goal of 0 angle to turn.
* Send a goal to sideward motion library to make the gate at horizontal center of the camera of robot. The data for current position is sent by Image Processing node of gate detection.
* When the robot is in the line of the center of the gate, goal is send to forward motion library to pass the robot from the gate.
* Stop stabilization and forward motion.

## Task Line 

This package is to detect and follow the lines. It will take the goal from master layer.

* Wait until goal is received from the master layer.
* Wait for motion library server to start.
* Start stabilization of turning motion by sending a goal of 0 angle to turn.
* Send a goal to forward motion library to move the robot forward to search line.
* Cancel the forward goal when line is found.
* Send sideward and forward goal to place the robot at the center of the line. The data for current position is sent by Image Processing node of line detection.
* Stop turning stabilization.
* Send a goal to the turn motion library to align the robot with line. The data for current position is sent by Image Processing node of line angle
