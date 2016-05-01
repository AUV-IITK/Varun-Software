#include <ros/ros.h>
#include <linefollowing/AlignAction.h>
#include <linefollowing/AlignActionFeedback.h>
#include <linefollowing/AlignActionResult.h>
#include <linedetection/orangeAction.h>
#include <linedetection/orangeActionFeedback.h>
#include <linedetection/orangeActionResult.h>
#include <actionlib/client/simple_action_client.h>
#include <actionlib/client/terminal_state.h>

typedef actionlib::SimpleActionClient<linedetection::orangeAction> orange;
typedef actionlib::SimpleActionClient<linefollowing::AlignAction> Align;

linedetection::orangeGoal orangegoal;
linefollowing::AlignGoal aligngoal;


bool orangeSuccess= false;
bool alignSuccess = false;

void spinThread(){
	ros::spin();
}

void forwardCb(linedetection::orangeActionFeedback msg){
	ROS_INFO("feedback recieved %d",msg.feedback.nosignificance);
}
void AlignCb(linefollowing::AlignActionFeedback msg){
	ROS_INFO("feedback recieved %fsec remaining ",msg.feedback.AngleRemaining);
}

int main(int argc, char** argv){
	ros::init(argc, argv,"TheMasterNode" );
	ros::NodeHandle nh;
	//here linedetectionserver is the name of the node of the actionserver.
	ros::Subscriber subOrange = nh.subscribe<linedetection::orangeActionFeedback>("/linedetectionserver/feedback",1000,&forwardCb);
	ros::Subscriber subAlign = nh.subscribe<linefollowing::AlignActionFeedback>("/Align/feedback",1000,&AlignCb);

	orange orangeClient("linedetectionserver");

	Align AlignClient("Align");

	ROS_INFO("Waiting for linedetection server to start.");
	orangeClient.waitForServer();
	ROS_INFO("linedetection server started");

	ROS_INFO("Waiting for align server to start.");
	AlignClient.waitForServer();
	ROS_INFO("Align server started.");

	boost::thread spin_thread(&spinThread);

	orangegoal.order = true;
	orangeClient.sendGoal(orangegoal);
	ROS_INFO("Orange detection started");
	orangeClient.waitForResult();
	orangeSuccess = (*(orangeClient.getResult())).MotionCompleted;
	if(orangeSuccess){
		ROS_INFO("orange colour detected");
	}
	else
		ROS_INFO("orange not detected");

	aligngoal.StartDetection = true;
	AlignClient.sendGoal(aligngoal);
	ROS_INFO("Alignment started");
	AlignClient.waitForResult();
	alignSuccess = (*(orangeClient.getResult())).MotionCompleted;
	if(alignSuccess){
		ROS_INFO("alignment successful");
	}
	else
		ROS_INFO("alignment failed");
	return 0;
}