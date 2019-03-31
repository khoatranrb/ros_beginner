#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <turtlesim/Pose.h>
#include <cmath>
#include <iostream>
#include <queue>
using namespace std;

int i = 0;
float x, y, theta, v, vt;
float tx = 0, ty = 5;
int state = 0, rate = 1000;
float a_z;
float target, target_angle, t_a;
double pi = 3.1415926535;
ros::Publisher pub;

geometry_msgs::Twist getMessage(double linear_x, double angular_z)
{
    geometry_msgs::Twist msg;
    msg.linear.x = linear_x;
    msg.angular.z = angular_z;
    return msg;
}

void move()
{
    //if()
    if (abs(t_a - theta) > pi / 4 && abs(t_a - theta) < 2 * pi - pi / 4)
    // if (t_a < 0.1 || t_a > 6.1)
    //     pub.publish(getMessage(1.5 * target, 0 * (t_a - theta)));
    // else
    {
        pub.publish(getMessage(0, 2 * a_z));
        //cout << "1" << endl;
    }
    else
    {
        if (abs(target) > 0.01)
        {
            if (abs(target) > 0.1)
                if (abs(t_a - theta) > 0.16)
                {
                    pub.publish(getMessage(2.4 * target, 0.2 * a_z));
                    //cout << "2" << endl;
                }
                else
                {
                    pub.publish(getMessage(2 * target, 0.2 * a_z));
                    //cout << "3" << endl;
                }
            else
            {
                if (abs(t_a - theta) > 0.016)
                {
                    pub.publish(getMessage(1 * target, 0.05 * a_z));
                    //cout << "4" << endl;
                }
                else
                {
                    pub.publish(getMessage(0.2 * target, 0.02 * a_z));
                    //cout << "5" << endl;
                }
            }
        }
        else
        {
            pub.publish(getMessage(0, 0));
            state = 0;
        }
    }
}

void poseCallback(const turtlesim::Pose::ConstPtr &msg)
{
    float prevx = x, prevy = y;
    x = msg->x, y = msg->y, theta = msg->theta,
    v = msg->linear_velocity, vt = msg->angular_velocity;
    tx += 0.0008;
    ty = 5 + sin(tx);
    cout << tx << endl
         << ty << endl
         << endl;
    // if (abs(t_a - theta) > pi / 4)
    // {
    //     cout << "a=" << t_a << endl
    //          << "t=" << target_angle << endl
    //          << "vt=" << vt << endl
    //          << endl;
    // };

    target = sqrt((tx - x) * (tx - x) + (ty - y) * (ty - y));
    target_angle = atan2(ty - y, tx - x);

    if (i != 0)
    {
        if (abs(target) > 0.01)
            state = 1;
        else
            state = 0;

        if (ty < y && tx < x)
            t_a = 2 * pi + target_angle;
        else
            t_a = target_angle;
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

void move(float x, float y)
{
    state = 1;
    target = sqrt((tx - x) * (tx - x) + (ty - y) * (ty - y));
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "myturtle_control");
    ros::NodeHandle h;
    pub = h.advertise<geometry_msgs::Twist>("turtle2/cmd_vel", 1000);
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
