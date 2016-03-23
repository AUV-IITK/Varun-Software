#include <ros/ros.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Int32.h>
#include <actionlib/server/simple_action_server.h>
#include <motion_actions/ForwardAction.h>
#include <dynamic_reconfigure/server.h>
#include <motion_forward/pidConfig.h>

#define minPWM 200 //min pwm at which thrusters move
#define maxPWM 240 //upper limit to control spped of bot
using namespace std;

typedef actionlib::SimpleActionServer<motion_actions::ForwardAction> Server; // defining the Client type

float presentForwardPosition=0;
float previousForwardPosition=0;
float finalForwardPosition, error, output;
bool initData =false;

std_msgs::Int32 pwm; // pwm to be send to arduino

// new inner class, to encapsulate the interaction with actionclient
class innerActionClass{
private:
	ros::NodeHandle nh_;
	Server forwardServer_;
	std::string action_name_;
	motion_actions::ForwardFeedback feedback_;
	motion_actions::ForwardResult result_;
	ros::Publisher PWM;
	float p,i,d;

public:
	//Constructor, called when new instance of class declared
	innerActionClass(std::string name):
		//Defining the server, third argument is optional
	   	forwardServer_(nh_, name, boost::bind(&innerActionClass::analysisCB, this, _1), false),
		action_name_(name)
	{
		// Add preempt callback
		forwardServer_.registerPreemptCallback(boost::bind(&innerActionClass::preemptCB, this));
		PWM = nh_.advertise<std_msgs::Int32>("/pwm/forward",1000);
		// Starting new Action Server
		forwardServer_.start();
	}

		// default contructor
		~innerActionClass(void){
		}

		// callback for goal cancelled
		// Stop the bot
	void preemptCB(void){
		pwm.data = 0;
		PWM.publish(pwm);
		ROS_INFO("pwm send to arduino %d", pwm.data);
		//this command cancels the previous goal
		// forwardServer_.setPreempted();
	}

		// called when new goal recieved
		// Start motion and finish it, if not interupted
	void analysisCB(const motion_actions::ForwardGoalConstPtr goal){
		ROS_INFO("Inside analysisCB");

		int loopRate =10 ;
		ros::Rate loop_rate(loopRate);

		//waiting till we recieve the first value from Camera else it's useless to to any calculations
		while(!initData){
			ROS_INFO("Waiting to get first input from Camera");
			loop_rate.sleep();
		}

		if(goal->Goal == 1)
			finalForwardPosition = 0;

		float derivative=0,integral=0,dt=1.0/loopRate;
		bool reached=false;

		pwm.data=0;

		if (!forwardServer_.isActive())
			return;

		while(!forwardServer_.isPreemptRequested()&&ros::ok()){
			error = finalForwardPosition - presentForwardPosition;
			integral+= (error*dt);
			derivative = (presentForwardPosition- previousForwardPosition)/dt;

			output= (p*error)+(i*integral)+(d*derivative);

			forwardOutputPWMMapping(output);

			//this lower limit depends upon the bot itself, below these values of PWM thrusters will not start
			if(mod(pwm.data) > maxPWM){
				if(pwm.data < 0)
					pwm.data = -maxPWM;
				else 
					pwm.data = maxPWM;
			}

			feedback_.DistanceRemaining = error;

			forwardServer_.publishFeedback(feedback_);
			PWM.publish(pwm);
			ROS_INFO("pwm send to arduino %d", pwm.data);
			
			ros::spinOnce();
			loop_rate.sleep();

			if(error<40 && error >-40){//assuming that this angle is in degree
				//write something to calculate the time we will wait for and check if we are in the range of 5 degrees
				// we can also check that if we are in this range and the angular velocity is also small then we can assume
				// that we are stable and we can now start moving 
				reached=true;
				pwm.data = 0;
				PWM.publish(pwm);
				ROS_INFO("thrusters stopped");
				break;
			}

			if (forwardServer_.isPreemptRequested() || !ros::ok())
			{
				ROS_INFO("%s: Preempted", action_name_.c_str());
				// set the action state to preempted
				forwardServer_.setPreempted();
				reached = false;
				break;
			}
		}

		if(reached){
			result_.Result = reached;
			ROS_INFO("%s: Succeeded", action_name_.c_str());
			// set the action state to succeeded
			forwardServer_.setSucceeded(result_);
		}
	}
	void forwardOutputPWMMapping(float output){
		float maxOutput=200, minOutput=-200,scale; //upper limit in terms of error ex. 200 cm of distance we will consider all distances > 100cm as 100cm

		scale = (maxPWM - minPWM)/(maxOutput-0);
		float temp;
		float bias;
		if(output > 0)
			bias = minPWM; //the minPWM;		
		else
			bias = -minPWM;

		temp = output*scale + bias;
		pwm.data = (int)temp;
	}
	int mod(int a){
		if(a<0)	return -a;
		else return a;
	}

void setPID(float new_p, float new_i, float new_d) {
		p=new_p;
		i=new_i;
		d=new_d;
	}
};
innerActionClass* object; 

///dynamic reconfig 
void callback(motion_forward::pidConfig &config, double level) {
	ROS_INFO("Reconfigure Request: p= %f i= %f d=%f", config.p, config.i, config.d);
	object->setPID(config.p, config.i, config.d);
}

void distanceCb(std_msgs::Float64 msg){
	//this is used to set the final angle after getting the value of first intial position
	if(initData == false){
		presentForwardPosition= msg.data;
		previousForwardPosition = presentForwardPosition;
		initData = true;
	}
	else{
		previousForwardPosition = presentForwardPosition;
		presentForwardPosition= msg.data;
		// ROS_INFO("New angular postion %f", msg.data);
	}
}

int main(int argc, char** argv){
	ros::init(argc, argv, "forward");

	ros::NodeHandle n;
	ros::Subscriber xDistance=n.subscribe<std_msgs::Float64>("xDistance",1000,&distanceCb);

	ROS_INFO("Waiting for Goal");
	object = new innerActionClass(ros::this_node::getName());

	//register dynamic reconfig server.
	dynamic_reconfigure::Server<motion_forward::pidConfig> server;
	dynamic_reconfigure::Server<motion_forward::pidConfig>::CallbackType f;
	f = boost::bind(&callback, _1, _2);
	server.setCallback(f);

	ros::spin();
	return 0;
}