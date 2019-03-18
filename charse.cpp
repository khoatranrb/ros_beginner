#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <turtlesim/Pose.h>
#include <iostream>
#include <queue>
using namespace std;

const double pi = 3.1415926535;
float x, y, theta, v, vt;
float x2, y2, theta2, v2, vt2;
int state = 0;
float target_angle;
float target_distance;
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
    if (abs(target_angle - theta2) < 0.01 || abs(abs(target_angle - theta2) - 2 * pi) < 0.01)
    {
        pub.publish(getMessage(0, 0));
        state = 0;
    }
    else
    {
        pub.publish(getMessage(0, 1));
    }
}

void handleStateStraightForward()
{
    // float dist = sqrt((x-prevx)*(x-prevx)+(y-prevy)*(y-prevy));
    // target_distance -= dist;
    // if (target_distance <= 0) {
    //     state = 0;
    //     pub.publish(getMessage(0, 0));
    // } else {
    //     pub.publish(getMessage(1, 0));
    // }
    // if (abs(x - x2) < 0.01 && abs(y - y2) < 0.01)
    // {
    //     pub.publish(getMessage(0, 0));
    //     state = 0;
    // }
    // else
    // {
    pub.publish(getMessage((x - x2) + (y - y2), 0));
    // }
}

void poseCallback(const turtlesim::Pose::ConstPtr &msg)
{
    float prevx = x, prevy = y;

    x = msg->x, y = msg->y, theta = msg->theta,
    v = msg->linear_velocity, vt = msg->angular_velocity;
}
void rotate(float angle)
{
    state = 1;
    target_angle = angle;
}

void poseCallback2(const turtlesim::Pose::ConstPtr &msg)
{
    float prevx2 = x2, prevy2 = y2;
    cout << "x=" << abs(x - x2) << endl;
    x2 = msg->x, y2 = msg->y, theta2 = msg->theta,
    v2 = msg->linear_velocity, vt2 = msg->angular_velocity;
    target_angle = atan2(y - y2, x - x2);
    cout << "x2=" << abs(y - y2) << endl;
    if (state == 1)
        handleStateRotate();

    if (state == 2)
        handleStateStraightForward();
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
    pub = h.advertise<geometry_msgs::Twist>("khoa/cmd_vel", 1000);
    ros::Subscriber sub =
        h.subscribe("/turtle1/pose", 1000, poseCallback);
    ros::Subscriber sub2 =
        h.subscribe("/khoa/pose", 1000, poseCallback2);
    ros::Rate loopRate(20);

    // bool in_action = false;
    while (ros::ok())
    {
        //pub.publish(getMessage(linear_x, 5.0));
        //linear_x += 1.0;

        queue<Action> q;
        Action a1, a2;
        a1.type = 1, a1.target_angle = atan2(y - y2, x - x2);
        a2.type = 2, a2.target_distance = sqrt((x - x2) * (x - x2) + (y - y2) * (y - y2));
        q.push(a1);
        q.push(a2);
        // if (state == 0 && !in_action) {
        if (q.size() > 0)
        {
            Action a = q.front();
            q.pop();
            if (a.type == 1)
                rotate(a.target_angle);
            else if (a.type == 2)
                straight_forward(a.target_distance);
            // in_action = true;
        }
        // if (state == 0 && in_action) {
        //     in_action = false;
        // }
        // if (abs(target_angle - theta2) > 0.01 || abs(abs(target_angle - theta2) - 2 * pi) > 0.01)
        // {
        //     cout<<"rotage"<<endl;
        //     state = 1;
        //     handleStateRotate();
        // }
        // else
        // {
        //     if (abs(x - x2) > 0.01 || abs(y - y2) > 0.01)
        //     {
        //         cout<<"active"<<endl;
        //         state = 2;
        //         handleStateStraightForward();
        //     }
        // }
        loopRate.sleep();
        ros::spinOnce();
    }

    return 0;
}
