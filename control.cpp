#include "ros/ros.h"
#include <math.h>
#include "geometry_msgs/Pose2D.h"										// to get desired position command
#include "turtlesim/Pose.h"												// to read current position
#include "geometry_msgs/Twist.h"										// to send velocity command

// Function declarations
void ComPoseCallback(const geometry_msgs::Pose2D::ConstPtr& msg);
void CurPoseCallback(const turtlesim::Pose::ConstPtr& msg);
float GetErrorLin(turtlesim::Pose curpose, geometry_msgs::Pose2D despose);
float GetErrorAng(turtlesim::Pose curpose, geometry_msgs::Pose2D despose);
geometry_msgs::Twist getMessage(double linear_x, double angular_z)
{
    geometry_msgs::Twist msg;
    msg.linear.x = linear_x;
    msg.angular.z = angular_z;
    return msg;
}

// Global variables
bool STOP = true;														// to hold stop flag, wait till first command given
//~ geometry_msgs::Twist CmdVel;
turtlesim::Pose CurPose;												// to hold current pose
geometry_msgs::Pose2D DesPose;											// variable to hold desired pose

int main(int argc, char **argv)
{
	ros::init(argc, argv, "TurtlesimPositionController_pubsub");		// connect to roscore
	ros::NodeHandle n;													// node object
	
	// register sub to get desired position/pose commands
	ros::Subscriber ComPose_sub = n.subscribe("/turtle1/PositionCommand", 5, ComPoseCallback);
	// register sub to get current position/pose
	ros::Subscriber CurPose_sub = n.subscribe("/turtle1/pose", 5, CurPoseCallback);
	// register pub to send twist velocity (cmd_vel)
	ros::Publisher pub = n.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 100);
	
	ros::Rate loopRate(0.5);											// freq to run loops in (10 Hz)
	float ErrorLin = 0;
	float ErrorAng = 0;
	
	ROS_INFO("Ready to send position commands");						// let user know we are ready and good
	while (ros::ok() && n.ok() )										// while ros and the node are ok
	{
		ros::spinOnce();
		pub.publish(getMessage(1,1));
			loopRate.sleep();
		if (STOP == false)												// and no stop command
		{
			ErrorLin = GetErrorLin(CurPose, DesPose);
			ErrorAng = GetErrorAng(CurPose, DesPose);
			printf("Error linear: %f, Error angular: %f\n", ErrorLin, ErrorAng);
		}
		else
		{
			printf("\nX : %f\nY : %f", CurPose.x, CurPose.y);
		}
		loopRate.sleep();												// sleep to maintain loop rate
	}
}


// call back to send new desired Pose msgs
void ComPoseCallback(const geometry_msgs::Pose2D::ConstPtr& msg)			
{
	STOP = false;														// start loop
	DesPose.x = msg->x;													// copy msg to varible to use elsewhere
	DesPose.y = msg->y;
	return;
}

// call back to send new current Pose msgs
void CurPoseCallback(const turtlesim::Pose::ConstPtr& msg)			
{
	CurPose.x = msg->x;
	CurPose.y = msg->y;
	CurPose.theta = msg->theta;											// copy msg to varible to use elsewhere
	return;
}

// function to get angular error between facing direction of the turtle and direction to desired pose
float GetErrorAng(turtlesim::Pose curpose, geometry_msgs::Pose2D despose)
{
	// create error vector
	float Ex = despose.x - curpose.x;									// Error X. X component 
	float Ey = despose.y - curpose.y;									// Error Y. Y component 
	
	// get desire angle
	float dest = atan2f(Ey, Ex); 										// use float version to get arc tangent
	
	// get angle error
	float Et = dest - curpose.theta;
	
	//~ ROS_INFO("Ex: %f, Ey: %f, Et: %f", Ex, Ey, Et);
	return Et;
}

// function to get linear error from the turtles perspective. Error only along turtle X axis
float GetErrorLin(turtlesim::Pose curpose, geometry_msgs::Pose2D despose)
{
	// create error vector
	float Ex = despose.x - curpose.x;									// Error X. X component
	float Ey = despose.y - curpose.y;									// Error Y. Y component 
	float Et = GetErrorAng(curpose, despose);							// get angle between vectors
	
	// project error onto turtle x axis
	//~ float Etx =  pow( pow(Ex,2.0) + pow(Ey,2.0), 0.5 )*cos(Et);
	float Etx = hypotf(Ex, Ey)*cos(Et); // improved c function
	
	return Etx;
}
