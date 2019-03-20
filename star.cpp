#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <turtlesim/Pose.h>
#include<cmath>
#include <iostream>
#include <queue>
using namespace std;

int i = 0;
float x, y, theta, v, vt;
int state = 0, rate = 30;
float dist;
float target_angle;
float target_distance;
double pi = 3.1415926535;
ros::Publisher pub;

geometry_msgs::Twist getMessage(double linear_x, double angular_z)
{
    geometry_msgs::Twist msg;
    msg.linear.x = linear_x;
    msg.angular.z = angular_z;
    return msg;
}

void handleStateRotate()
{
    if (abs(target_angle - theta) > 0.01)
    {
        if (abs(target_angle - theta) > 1.0 / rate)
            pub.publish(getMessage(0, target_angle - theta));
        else
        {
            pub.publish(getMessage(0, rate * (target_angle - theta)));
        }
    }
    else
    {
        pub.publish(getMessage(0, 0));
        state = 0;
    }
}

void handleStateStraightForward()
{

    if (target_distance <= 0)
    {
        state = 0;
        pub.publish(getMessage(0, 0));
    }
    else
    {
        if (target_distance > 1.0 / rate)
            pub.publish(getMessage(1, 0));
        else
            pub.publish(getMessage(target_distance * rate, 0));
    }
}

void poseCallback(const turtlesim::Pose::ConstPtr &msg)
{
    static bool firstCall = true;
    float prevx = x, prevy = y;
    x = msg->x, y = msg->y, theta = msg->theta,
    v = msg->linear_velocity, vt = msg->angular_velocity;
    dist = sqrt((x - prevx) * (x - prevx) + (y - prevy) * (y - prevy));
    if (!firstCall)
        target_distance -= dist;
    firstCall = false;
}

void rotate(float angle)
{
    state = 1;
    target_angle = angle;
}

void straight_forward(float distance)
{
    state = 2;
    target_distance = distance;
}

struct Action
{
    int type;
    float target_angle;
    float target_distance;
};

int main(int argc, char **argv)
{
    ros::init(argc, argv, "myturtle_control");
    ros::NodeHandle h;
    pub = h.advertise<geometry_msgs::Twist>("turtle1/cmd_vel", 1000);
    ros::Subscriber sub =
        h.subscribe("/turtle1/pose", 1000, poseCallback);
    ros::Rate loopRate(rate);

    queue<Action> q;
    q.push({1, acos(sin(54*pi/180)), 0});
    q.push({2, 0, 4});
    q.push({1, pi, 0});
    q.push({2, 0, 4});
    q.push({1, 2*pi-acos(sin(54*pi/180)),0});
    q.push({2, 0, 4});
    q.push({1, pi-acos(sin(54*pi/180))-2*asin(0.25/sin(54*pi/180)), 0});
    q.push({2, 0, 4});
    q.push({1, pi+acos(sin(54*pi/180))+36*pi/180, 0});
    q.push({2, 0, 4});
    q.push({1,0,0});
    q.push({1, acos(sin(54*pi/180)), 0});
    q.push({2, 0, 4});
    q.push({1, pi, 0});
    q.push({2, 0, 4});
    q.push({1, 2*pi-acos(sin(54*pi/180)),0});
    q.push({2, 0, 4});
    q.push({1, pi-acos(sin(54*pi/180))-2*asin(0.25/sin(54*pi/180)), 0});
    q.push({2, 0, 4});
    q.push({1, pi+acos(sin(54*pi/180))+36*pi/180, 0});
    q.push({2, 0, 4});
    // q.push({1, 75*pi/180, 0});
    // q.push({2, 0, 4});
    // q.push({1, 3*pi/2+15*pi/180, 0});
    // q.push({2, 0, 4});
    // q.push({1, 135*pi/180,0});
    // q.push({2, 0, 4});
    // q.push({1, 0, 0});
    // q.push({2, 0, 4});
    // q.push({1, pi+pi/6, 0});
    // q.push({2, 0, 4});
    bool in_action = false;
    while (ros::ok())
    {
        //pub.publish(getMessage(linear_x, 5.0));
        //linear_x += 1.0;
        if (state == 0 && !in_action)
        {
            if (q.size() > 0)
            {
                Action a = q.front();
                q.pop();
                if (a.type == 1)
                    rotate(a.target_angle);
                else if (a.type == 2)
                    straight_forward(a.target_distance);
                in_action = true;
            }
        }
        else if (state == 0 && in_action)
        {
            in_action = false;
        }
        else if (state == 1)
        {
            handleStateRotate();
        }

        else if (state == 2)
        {
            handleStateStraightForward();
        }
        loopRate.sleep();
        ros::spinOnce();
    }

    return 0;
}
