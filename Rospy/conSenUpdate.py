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
        target_angular = 0
       # target_theta = atan2((ty-self.pose.y), (tx-self.pose.x))
       # target_theta = math.atan((ty-self.pose.y)/(tx-self.pose.x))
        while not rospy.is_shutdown():

            if(action == 0):
                lenh = [i for i in input("Ra lenh cho toi : ").rsplit(" ", 1)]
                if(lenh[0].lower() == 'quay trai'):
                    target_angular = float(lenh[1])
                    msg.angular.z = 1
                    action = 1
                elif(lenh[0].lower() == 'quay phai'):
                    target_angular = float(lenh[1])
                    msg.angular.z = -1
                    action = 1
                elif(lenh[0].lower() == 'tien'):
                    target_distance = float(lenh[1])
                    msg.linear.x = 1
                    action = 2
                elif(lenh[0].lower() == 'lui'):
                    target_distance = float(lenh[1])
                    msg.linear.x = -1
                    action = 2
                else:
                    print("what the hell?")

                #target_angular = float(input("target angular = "))
                t_a = pi*target_angular/180 + self.pose.theta
                while(t_a < 0):
                    t_a = 2*pi+t_a
                while(t_a > 2*pi):
                    t_a -= 2*pi

            if(action == 1):
                t0 = float(rospy.Time.now().to_sec())
                rotated = 0
                while(rotated < target_angular):

                    self.pub.publish(msg)
                    t1 = float(rospy.Time.now().to_sec())
                    rotated = abs(msg.angular.z)*(t1-t0)*180/pi
                msg.angular.z = 0
                self.pub.publish(msg)
                action = 0
                print("Da hoan thanh!")
            elif(action == 2):
                t0 = float(rospy.Time.now().to_sec())
                distance = 0
                while(target_distance > distance):

                    self.pub.publish(msg)
                    t1 = float(rospy.Time.now().to_sec())
                    distance = abs(msg.linear.x)*(t1-t0)
                msg.linear.x = 0
                self.pub.publish(msg)
                action = 0
                print("Da hoan thanh!")

        self.rate.sleep()
        rospy.spin()


if __name__ == '__main__':
    try:
        x = turtlebot()
        x.move()
    except rospy.ROSInterruptException:
        pass
