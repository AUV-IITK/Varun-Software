# Master Layer

This is the main package which invoke all the tasks according to the arena. This node automates all the tasks.

* Wait for all the task handler servers to start.
* The procedure is followed until success or failure.
* Send a goal to the task line to find and follow the line.
* After finding line, it sends a goal to task buoy to hit the desired buoy.
* It again search for line to follow, and hence sends another goal to task line.
* Kill if line not found.
* Sends a goal to task gate to pass the gate.
