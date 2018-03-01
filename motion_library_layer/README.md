# Motion Library Layer

This layer is responsible for the forward, sideward, turn and upward motions of the robot. All the four motions use similar algorithm..

* Wait until goal is received from the task handlers.
* On receiving the goal, this procedure is followed until the completion or failure of
the task.
* Kill if server is not active or goal is cancelled.
* Subscribe current position from the sensors and compute difference from final
position.
* Send difference as feedback.
* Apply PID on the difference to get the output value.
* Scale output value in the PWM range (0-255).
* If PWM value is between band range for a certain number of times specified by goal, stop the motion, else publish the PWM to a topic.
