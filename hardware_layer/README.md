# Hardware Layer

## Micro-controller

__Hardware__: Arduino Mega 2560

__Purpose__:  Subscribes to the PWM values published from the [motion_library_layer](https://github.com/AUV-IITK/auv2016/tree/master/motion_library_layer) and sends it to the Arduino to apply variable voltages on thrusters accordingly, also publishes data from the depth sensor

* Subscribe data on four different topics for four different motions. And publish
pressure sensor data on a topic.
* When a PWM is subscribed on a topic, it’s callback function is called. It first
apply calibration on the PWM to get the individual PWM of each thruster, then
send it to the thrusters.
* In case of turn motion, it uses side thursters if robot is moving sideward also, else
it uses thursters at end.

## Camera and Image Processing

__Hardware__: Logitech Webcam

__Purpose__: Takes the raw data from the two cameras (front and bottom) and publish them on two different topics for processing on task
nodes. It is done for each frame at a certain rate.

## Inertial Measurement Unit (IMU)

__Hardware__: [Navstick Nanopilot Board](https://github.com/AUV-IITK/AUVWiki/wiki/Using-NavStick-NanoPilot-Board)

__Purpose__: Publish the angle from the North axis.

* Takes data from the navstik port.
* Runs calculations to compute the angle from North axis.
* Publishes the angle on a topic to motion library and task handlers.
