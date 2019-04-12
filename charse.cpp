#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <turtlesim/Pose.h>
#include <cmath>
#include <iostream>
#include <algorithm>
using namespace std;

int i = 0;
float x, y, theta, v, vt;
float tx = 2, ty = 1.5;
int state = 0, rate = 600;
float a_z;
float target, target_angle, t_a;
double pi = 3.1415926535;
ros::Publisher pub;

geometry_msgs::Twist getMessage(float linear_x, float angular_z)
{
    geometry_msgs::Twist msg;
    msg.linear.x = linear_x;
    msg.angular.z = angular_z;
    return msg;
}

void poseCallback(const turtlesim::Pose::ConstPtr &msg)
{
    x = msg->x, y = msg->y, theta = msg->theta,
    v = msg->linear_velocity, vt = msg->angular_velocity;
    // if (abs(t_a - theta) > pi / 4)
    // {
    //     cout << "a=" << t_a << endl
    //          << "t=" << target_angle << endl
    //          << "vt=" << vt << endl
    //          << endl;
    // };
    if (v == 0 && vt == 0)
        cout << "Stop" << endl;
    if (theta < 0)
        theta = 2 * pi + theta;
    target = sqrt((tx - x) * (tx - x) + (ty - y) * (ty - y));
    target_angle = atan2(ty - y, tx - x);

    if (i != 0)
    {
        if (target > 0.00001)
            state = 1;
        else
            state = 0;

        t_a = target_angle;
        if (target_angle < 0)
        {
            t_a = 2 * pi + target_angle;
        }
    }
    else
    {
        t_a = 0;
        target = 0.1;
        state = 0;
        i++;
    }
    if (t_a < pi)
    {
        if (theta < t_a + pi)
            a_z = t_a - theta;
        else
            a_z = theta - t_a;
    }
    else
    {
        if (theta < t_a - pi)
            a_z = theta - t_a;
        else
            a_z = t_a - theta;
    }
}

float angularZ()
{
    if (a_z > 0)
        return min(float(2), a_z);
    else
        return max(a_z, float(-2));
}
float linearX()
{
    return min(float(4), target);
}

void move()
{
    if (target > 0.00001)
    {
        pub.publish(getMessage(linearX(), 2 * angularZ()));
        cout << "1" << endl;
    }
    else
    {
        pub.publish(getMessage(0, 0));
        state = 0;
    }
}

void poseCallback2(const turtlesim::Pose::ConstPtr &msg)
{

    tx = msg->x, ty = msg->y;
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "myturtle_control");
    ros::NodeHandle h;
    pub = h.advertise<geometry_msgs::Twist>("turtle2/cmd_vel", 1000);
    ros::Subscriber sub2 =
        h.subscribe("/turtle1/pose", 1000, poseCallback2);
    ros::Subscriber sub =
        h.subscribe("/turtle2/pose", 1000, poseCallback);
    ros::Rate loopRate(rate);

    bool in_action = false;
    while (ros::ok())
    {
        if (state == 1)
        {
            move();
        }
        loopRate.sleep();
        ros::spinOnce();
    }

    return 0;
}
