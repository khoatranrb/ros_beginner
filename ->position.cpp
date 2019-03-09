#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include<cmath>
#include<iostream>
using namespace std;
const double pi=3.14;
geometry_msgs::Twist getMessage(double linear_x, double angular_z)
{
    geometry_msgs::Twist msg;
    msg.linear.x = linear_x;
    msg.angular.z = angular_z;
    return msg;
}
int main(int argc, char** argv)
{
    ros::init(argc, argv, "myturtle_control");
    ros::NodeHandle h;
    ros::Publisher pub = 
        h.advertise<geometry_msgs::Twist>("turtle1/cmd_vel", 1000);
    ros::Rate loopRate(0.5);
    int x,y;
    cin>>x>>y;
    //pub.publish(getMessage(0,0));
    loopRate.sleep();
    if(x==0 && y>0) {loopRate.sleep();pub.publish(getMessage(0,pi/2)); loopRate.sleep();}
    else if(x==0 && y<=0){loopRate.sleep(); pub.publish(getMessage(0,-pi/2));loopRate.sleep();}
    else if(x>0 && y<0){loopRate.sleep(); pub.publish(getMessage(0,atan(double(y)/x)));loopRate.sleep();}
    else if(x<0 && y>0){loopRate.sleep(); pub.publish(getMessage(0,pi+atan(double(y)/x)));loopRate.sleep();}
    else if(x<0 && y<0){loopRate.sleep(); pub.publish(getMessage(0,pi+atan(double(y)/x)));loopRate.sleep();}
    else {loopRate.sleep();pub.publish(getMessage(0,atan(double(y)/x)));loopRate.sleep();}
    loopRate.sleep();
    pub.publish(getMessage(sqrt(x*x+y*y),0));
    loopRate.sleep();
    pub.publish(getMessage(0,0));
    loopRate.sleep();
    return 0;
    }
