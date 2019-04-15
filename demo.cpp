#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <turtlesim/Pose.h>
#include <cmath>
#include <iostream>
#include <algorithm>
using namespace std;

int i = 0;
float prevx, prevy, prevtheta;
float x = 5.5444, y = 5.5444, theta, v, vt;
float tx = 2, ty = 1.5;
int state = 0, rate = 600;
float a_z;
float dist = 0, ds, distances = 0;
float ox, oy, w = 10, w_theta, d_theta;
float target, target_angle, t_a;
float angle, angles, da;
double pi = 3.1415926535;
ros::Publisher pub;

geometry_msgs::Twist getMessage(float linear_x, float angular_z);
void poseCallback(const turtlesim::Pose::ConstPtr &msg);
void poseCallback2(const turtlesim::Pose::ConstPtr &msg);
bool vatCan();
void dor()
{
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
    if (vatCan() == true)
        return 1;
    else
    {
        if (a_z > 0)
            return min(float(4), 2 * a_z);
        else
            return max(2 * a_z, float(-4));
    }
}
float linearX()
{
    if (vatCan() == true)
        return 0;
    else
        return min(float(4), target);
}

void move()
{
    if (target > 0.00001)
    {
        pub.publish(getMessage(linearX(), angularZ()));
        //cout << "1" << endl;
    }
    else
    {
        pub.publish(getMessage(0, 0));
        state = 0;
    }
}

void straightForward()
{
    ds = abs(distances - dist);
    if (ds < 0.00001)
    {
        pub.publish(getMessage(0, 0));
        state = 0;
    }
    else
    {
        pub.publish(getMessage(min(float(4), ds), 0));
    }
}

void rotate()
{
    da = abs(angles - theta);
    if (da < 0.00001)
    {
        pub.publish(getMessage(0, 0));
        state = 0;
    }
    else
    {
        pub.publish(getMessage(0, angularZ()));
    }
}
int main(int argc, char **argv)
{
    ros::init(argc, argv, "myturtle_control");
    ros::NodeHandle h;
    pub = h.advertise<geometry_msgs::Twist>("turtle1/cmd_vel", 1000);
    ros::Subscriber sub =
        h.subscribe("/turtle1/pose", 1000, poseCallback);
    ros::Subscriber sub2 =
        h.subscribe("/turtle2/pose", 1000, poseCallback2);
    ros::Rate loopRate(rate);

    bool in_action = false;
    while (ros::ok())
    {
        if (state == 0 && !in_action)
        {
            cout << "Choose command : \n1. Move to target.\n2. Straight Forward.\n3. Rotate. \nYour choose: ";
            cin >> state;
            if (state == 1)
            {
                cout << "tx = ";
                cin >> tx;
                cout << "ty = ";
                cin >> ty;
            }
            else if (state == 2)
            {
                cout << "Target distance: ";
                cin >> distances;
            }
            else if (state == 3)
            {
                cout << "Target angle: ";
                cin >> angles;
            }
            dist = 0;
            in_action = true;
        }
        else if (state == 1)
        {
            move();
        }
        else if (state == 2)
        {
            straightForward();
        }
        else if (state == 3)
        {
            rotate();
        }
        else if (state == 0 && in_action)
        {
            cout << dist << endl;
            in_action = false;
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
    prevx = x;
    prevy = y;
    x = msg->x, y = msg->y, theta = msg->theta,
    v = msg->linear_velocity, vt = msg->angular_velocity;
    //if (v == 0 && vt == 0)
    //cout << "Stop" << endl;
    if (theta < 0)
        theta = 2 * pi + theta;
    target = sqrt((tx - x) * (tx - x) + (ty - y) * (ty - y));
    target_angle = atan2(ty - y, tx - x);

    if (i != 0)
    {
        dist += sqrt((prevx - x) * (prevx - x) + (prevy - y) * (prevy - y));
        if (state == 3)
            t_a = angles;
        else
        {
            t_a = target_angle;
            if (target_angle < 0)
            {
                t_a = 2 * pi + target_angle;
            }
        }
    }
    else
    {
        t_a = 0;
        target = 0;
        i = 1;
    }
    dor();

    // if (vatCan() == true)
    //     cout << "Co vat can " << d_theta << endl;
    // else
    //     cout << "fine" << endl;
}

void poseCallback2(const turtlesim::Pose::ConstPtr &msg)
{
    ox = msg->x, oy = msg->y;
    w = sqrt((ox - x) * (ox - x) + (oy - y) * (oy - y));
    w_theta = atan2(oy - y, ox - x);
    if (w_theta < 0)
        w_theta += 2 * pi;
    d_theta = abs(w_theta - theta);
    if (d_theta > pi)
        d_theta = 2 * pi - d_theta;
}

bool vatCan()
{
    if (w < 1 && (d_theta < pi / 6 || d_theta > 2 * pi - pi / 6))
        return true;
    else
        return false;
}
