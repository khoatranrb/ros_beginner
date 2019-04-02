#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <turtlesim/Pose.h>
#include <cmath>
#include <iostream>
#include <queue>
using namespace std;

int i = 0;
int tx1,ty1,tx2,ty2,tx3,ty3;
float x1, y1, theta1, v1, vt1;
float x2, y2, theta2, v2, vt2;
float x3, y3, theta3, v3, vt3;
int state = 0, rate = 600;
float a_z1,a_z2,a_z3;
float target, target_angle, t_a;
float target2, target_angle2, t_a2;
float target3, target_angle3, t_a3;
double pi = 3.1415926535;
ros::Publisher pub1,pub2,pub3;

geometry_msgs::Twist getMessage(double linear_x, double angular_z)
{
    geometry_msgs::Twist msg;
    msg.linear.x = linear_x;
    msg.angular.z = angular_z;
    return msg;
}

void move2()
{
    if (target2 > 0.00001)
    {
        if (target2 > 0.1)
        {
            if (abs(t_a2 - theta2) > pi / 4 && abs(t_a2 - theta2) < 2 * pi - pi / 4)
            {
                pub2.publish(getMessage(0, 2 * a_z2 / abs(a_z2)));
                cout << "1" << endl;
            }
            else
            {
                pub2.publish(getMessage(2, 1 * a_z2 / abs(a_z2)));
                cout << "2" << endl;
            }
        }
        else
        {
            pub2.publish(getMessage(target2, a_z2));
            cout << "3" << endl;
        }
    }
    else
    {
        pub2.publish(getMessage(0, 0));
        state = 0;
    }
}

void poseCallback2(const turtlesim::Pose::ConstPtr &msg)
{
    x2 = msg->x, y2 = msg->y, theta2 = msg->theta,
    v2 = msg->linear_velocity, vt2 = msg->angular_velocity;
    // if (abs(t_a - theta) > pi / 4)
    // {
    //     cout << "a=" << t_a << endl
    //          << "t=" << target_angle << endl
    //          << "vt=" << vt << endl
    //          << endl;
    // };
    if (v2 == 0 && vt2 == 0)
        cout << "Stop" << endl;
    if (theta2 < 0)
        theta2 = 2 * pi + theta2;
    target2 = sqrt((tx2 - x2) * (tx2 - x2) + (ty2 - y2) * (ty2 - y2));
    target_angle2 = atan2(ty2 - y2, tx2 - x2);

    if (i != 0)
    {
        if (target2) > 0.00001)
            state = 1;
        else
            state = 0;

        t_a = target_angle;
        if (target_angle2 < 0)
        {
            t_a2 = 2 * pi + target_angle2;
        }
        // if (ty < y && tx < x)
        //     t_a = 2 * pi + target_angle;
        // else
        //     t_a = target_angle;
    }
    else
    {
        t_a2 = 0;
        target2 = 0.1;
        state = 0;
        i++;
    }
    if (t_a2 < pi)
    {
        if (theta2 < t_a2 + pi)
            a_z2 = t_a2 - theta2;
        else
            a_z2 = theta2 - t_a2;
    }
    else
    {
        if (theta2 < t_a2 - pi)
            a_z2 = theta2 - t_a2;
        else
            a_z2 = t_a2 - theta2;
    }
    tx3 = msg->x, ty3 = msg->y;
}

void poseCallback1(const turtlesim::Pose::ConstPtr &msg)
{

    tx = msg->x, ty = msg->y;
}
void move(float x, float y)
{
    state = 1;
    target = sqrt((tx - x) * (tx - x) + (ty - y) * (ty - y));
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "myturtle_control");
    ros::NodeHandle h;
    pub1 = h.advertise<geometry_msgs::Twist>("turtle1/cmd_vel", 1000);
    pub2 = h.advertise<geometry_msgs::Twist>("turtle2/cmd_vel", 1000);
    pub3 = h.advertise<geometry_msgs::Twist>("turtle3/cmd_vel", 1000);
    ros::Subscriber sub1 =
        h.subscribe("/turtle1/pose", 1000, poseCallback1);
    ros::Subscriber sub2 =
        h.subscribe("/turtle2/pose", 1000, poseCallback2);
    ros::Subscriber sub3 =
        h.subscribe("/turtle3/pose", 1000, poseCallback3);
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
