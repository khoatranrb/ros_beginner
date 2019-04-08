#!/usr/bin/env python

import rospy
from geometry_msgs.msg import Twist
from turtlesim.msg import Pose
from math import pow, atan2, sqrt
import math
pi = 3.1415926535897
rate = 300


class turtlebot():
    def __init__(self):
        # Creating our node,publisher and subscriber
        rospy.init_node('turtlebot_controller', anonymous=True)
        self.pub = rospy.Publisher(
            '/turtle1/cmd_vel', Twist, queue_size=10)
        self.pose_subscriber = rospy.Subscriber(
            '/turtle1/pose', Pose, self.callback)
        self.pose = Pose()
        self.rate = rospy.Rate(rate)

    def callback(self, data):
        self.pose = data
        if(self.pose.theta < 0):
            self.pose.theta = 2*pi+self.pose.theta

    def move(self):
        i = 0
        msg = Twist()
        action = 0
       # target_theta = atan2((ty-self.pose.y), (tx-self.pose.x))
       # target_theta = math.atan((ty-self.pose.y)/(tx-self.pose.x))
        while not rospy.is_shutdown():
            if(action == 0):
                tx = float(input("tx = "))
                ty = float(input("ty = "))
                action = 1
            target_theta = atan2((ty-self.pose.y), (tx-self.pose.x))
            distance = sqrt(pow(ty-self.pose.y, 2)+pow(tx-self.pose.x, 2))

            if(i != 0):
                t_a = target_theta
                if(t_a < 0):
                    t_a = 2*pi+target_theta
            else:
                t_a = 0
                distance = 0.0001
                i = 1

            if(t_a < pi):
                if (self.pose.theta < t_a + pi):
                    a_z = t_a - self.pose.theta
                else:
                    a_z = self.pose.theta - t_a
            else:
                if (self.pose.theta < t_a - pi):
                    a_z = -t_a + self.pose.theta
                else:
                    a_z = -self.pose.theta + t_a

            if(distance > 0.00001):
                if(distance > 0.1):
                    if(abs(t_a-self.pose.theta) > pi/4 and abs(t_a-self.pose.theta) < 2*pi-pi/4):
                        msg.linear.x = 0
                        msg.angular.z = 2*a_z/abs(a_z)
                        self.pub.publish(msg)
                        print("1")
                    else:
                        msg.linear.x = 2
                        msg.angular.z = 1*a_z/abs(a_z)
                        self.pub.publish(msg)
                        print("2")
                else:
                    msg.linear.x = distance
                    msg.angular.z = a_z
                    self.pub.publish(msg)
                    print("3")
            else:
                msg.linear.x = 0
                msg.angular.z = 0
                self.pub.publish(msg)
                action = 0
                print("Stop")

        self.rate.sleep()
        rospy.spin()


if __name__ == '__main__':
    try:
        x = turtlebot()
        x.move()
    except rospy.ROSInterruptException:
        pass
