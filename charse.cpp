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
float ox, oy, w = 10, w_theta, d_theta;
float target, target_angle, t_a;
double pi = 3.1415926535;
ros::Publisher pub;

geometry_msgs::Twist getMessage(float linear_x, float angular_z);
void poseCallback(const turtlesim::Pose::ConstPtr &msg);
void poseCallback2(const turtlesim::Pose::ConstPtr &msg);
bool vatCan();
float angularZ()
{
    if (a_z > 0)
        return min(float(4), 2 * a_z);
    else
        return max(2 * a_z, float(-4));
}
float linearX()
{
    return min(float(4), target);
}
void dor()
{
    if (state == 2)
    {
        t_a += pi;
        if (t_a > 2 * pi)
            t_a -= 2 * pi;
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
    if ((t_a < pi / 6 || t_a > 2 * pi - pi / 6) && (theta < pi / 6 || theta > 2 * pi - pi / 6))
    {
        if (t_a > pi)
            t_a -= 2 * pi;
        if (theta > pi)
            theta -= 2 * pi;
        a_z = t_a - theta;
    }
}

void move()
{
    if (target > 0.00001)
    {
        if (state == 1)
        {
            pub.publish(getMessage(linearX(), angularZ()));
            //cout << "1" << endl;
        }
        else if (state == 2)
        {
            pub.publish(getMessage(-linearX(), angularZ()));
            //cout << "1" << endl;
        }
    }
    else
    {
        pub.publish(getMessage(0, 0));
        state = 0;
    }
}
void poseCallback1(const turtlesim::Pose::ConstPtr &msg)
{
    tx = msg->x;
    ty = msg->y;
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "myturtle_control");
    ros::NodeHandle h;
    pub = h.advertise<geometry_msgs::Twist>("turtle2/cmd_vel", 1000);
    ros::Subscriber sub =
        h.subscribe("/turtle2/pose", 1000, poseCallback);
    ros::Subscriber sub1 =
        h.subscribe("/turtle1/pose", 1000, poseCallback1);
    ros::Rate loopRate(rate);

    bool in_action = false;
    while (ros::ok())
    {
        if (state == 1 || state == 2)
        {
            move();
        }
        loopRate.sleep();
        ros::spinOnce();
    }

    return 0;
}

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
    //if (v == 0 && vt == 0)
    //cout << "Stop" << endl;
    if (theta < 0)
        theta = 2 * pi + theta;
    target = sqrt((tx - x) * (tx - x) + (ty - y) * (ty - y));
    target_angle = atan2(ty - y, tx - x);
    if (target < 0.00001)
        state = 0;
    else
    {
        if (abs(target_angle - theta) > pi / 2 && abs(target_angle - theta) < 3 * pi / 2)
            state = 2;
        else
            state = 1;
        if (i != 0)
        {
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
            i++;
        }
        dor();
    }
}
