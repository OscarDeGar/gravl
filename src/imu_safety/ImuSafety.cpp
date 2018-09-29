#include "ImuSafety.h"
#include <tf/tf.h>

ImuSafety::ImuSafety()
  : pub(n.advertise<geometry_msgs::Vector3>("safe_alpha", 1000))
  , sub(n.subscribe("/imu/data", 1000, &ImuSafety::ImuSafety::callback, this))
  , rate(ros::Rate(10))
{
  n.param<double>("maxThetaZ", max_roll, 10);
}

void ImuSafety::callback(const sensor_msgs::Imu::ConstPtr& msg)
{
  auto o = msg->orientation;
  tf::Quaternion q(o.x, o.y, o.z, o.w);
  double dummy_pitch, dummy_roll;
  ((tf::Matrix3x3) q).getRPY(pub_val.theta, dummy_pitch, dummy_roll);
  pub_val.danger = abs(pub_val.theta) > 0.1745;
}

void ImuSafety::spin()
{
  while (ros::ok())
    {
      pub.publish(pub_val);
      ros::spinOnce();
      rate.sleep();
    }
}

int main(int argc, char** argv)
{
  ros::init(argc, argv, "ImuSafety");
  ImuSafety imu_safety;
  imu_safety.spin();
}
