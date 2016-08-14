// Copyright 2016 AUV-IITK
#include <cv.h>
#include <highgui.h>
#include <ros/ros.h>
#include "std_msgs/String.h"
#include "std_msgs/Int8.h"
#include <fstream>
#include <vector>
#include <std_msgs/Bool.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>
#include <image_transport/image_transport.h>
#include "std_msgs/Float64MultiArray.h"
#include <cv_bridge/cv_bridge.h>
#include <sstream>
#include <string>

int w = -2, x = -2, y = -2, z = -2;
bool IP = true;
bool flag = false;
bool video = false;
int t1min = 0, t1max = 130, t2min = 0, t2max = 100, t3min = 150, t3max = 260;  // Default Params

cv::Mat frame;
cv::Mat newframe;
int count = 0, count_avg = 0;

void gateListener(std_msgs::Bool msg)
{
  IP = msg.data;
}

void imageCallback(const sensor_msgs::ImageConstPtr &msg)
{
  try
  {
    count++;
    newframe = cv_bridge::toCvShare(msg, "bgr8")->image;
    cvNamedWindow("newframe", CV_WINDOW_NORMAL);
    ///////////////////////////// DO NOT REMOVE THIS, IT COULD BE INGERIOUS TO HEALTH /////////////////////
    newframe.copyTo(frame);
    cv::imshow("newframe", newframe);
    ////////////////////////// FATAL ///////////////////////////////////////////////////
  }
  catch (cv_bridge::Exception &e)
  {
    ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
  }
}

int main(int argc, char* argv[])
{
  int height, width, step, channels;  // parameters of the image we are working on
  std::string Video_Name = "Random_Video";
  if (argc >= 2)
    flag = true;
  if (argc == 3)
  {
    video = true;
    std::string avi = ".avi";
    Video_Name = (argv[2]) + avi;
  }

  cv::VideoWriter output_cap(Video_Name, CV_FOURCC('D', 'I', 'V', 'X'), 9, cv::Size(640, 480));

  ros::init(argc, argv, "gate_detection");
  ros::NodeHandle n;

  ros::Publisher pub = n.advertise<std_msgs::Float64MultiArray>("/varun/ip/gate", 1000);
  ros::Subscriber sub = n.subscribe<std_msgs::Bool>("gate_detection_switch", 1000, &gateListener);
  ros::Rate loop_rate(10);

  image_transport::ImageTransport it(n);
  image_transport::Subscriber sub1 = it.subscribe("/varun/sensors/front_camera/image_raw", 1, imageCallback);

  cvNamedWindow("After Color Filtering", CV_WINDOW_NORMAL);
  cvNamedWindow("Contours", CV_WINDOW_NORMAL);
  cvNamedWindow("RealPic", CV_WINDOW_NORMAL);

  if (flag)
  {
    cvNamedWindow("F1", CV_WINDOW_NORMAL);
    cvNamedWindow("F2", CV_WINDOW_NORMAL);
    cvNamedWindow("F3", CV_WINDOW_NORMAL);

    cvCreateTrackbar("t1min", "F1", &t1min, 260, NULL);
    cvCreateTrackbar("t1max", "F1", &t1max, 260, NULL);
    cvCreateTrackbar("t2min", "F2", &t2min, 260, NULL);
    cvCreateTrackbar("t2max", "F2", &t2max, 260, NULL);
    cvCreateTrackbar("t3min", "F3", &t3min, 260, NULL);
    cvCreateTrackbar("t3max", "F3", &t3max, 260, NULL);
  }


  // capture size -
  CvSize size = cvSize(width, height);

  // Initialize different images that are going to be used in the program
  cv::Mat hsv_frame, thresholded, thresholded1, thresholded2, thresholded3, filtered;  // image converted to HSV plane
  // asking for the minimum distance where bwe fire torpedo

  while (ros::ok())
  {
    std_msgs::Float64MultiArray array;
    loop_rate.sleep();

    // Get one frame
    if (frame.empty())
    {
      std::cout << "empty frame \n";
      ros::spinOnce();
      continue;
    }

    if (video)
      output_cap.write(frame);

    // get the image data
    height = frame.rows;
    width = frame.cols;
    step = frame.step;

    // Covert color space to HSV as it is much easier to filter colors in the HSV color-space.
    cv::cvtColor(frame, hsv_frame, CV_BGR2HSV);

    cv::Scalar hsv_min = cv::Scalar(t1min, t2min, t3min, 0);
    cv::Scalar hsv_max = cv::Scalar(t1max, t2max, t3max, 0);

    // Filter out colors which are out of range.
    cv::inRange(hsv_frame, hsv_min, hsv_max, thresholded);
    // Split image into its 3 one dimensional images
    cv::Mat thresholded_hsv[3];
    cv::split(hsv_frame, thresholded_hsv);

    // Filter out colors which are out of range.
    cv::inRange(thresholded_hsv[0], cv::Scalar(t1min, 0, 0, 0), cv::Scalar(t1max, 0, 0, 0), thresholded_hsv[0]);
    cv::inRange(thresholded_hsv[1], cv::Scalar(t2min, 0, 0, 0), cv::Scalar(t2max, 0, 0, 0), thresholded_hsv[1]);
    cv::inRange(thresholded_hsv[2], cv::Scalar(t3min, 0, 0, 0), cv::Scalar(t3max, 0, 0, 0), thresholded_hsv[2]);

    cv::GaussianBlur(thresholded, thresholded, cv::Size(9, 9), 0, 0, 0);
    cv::imshow("After Color Filtering", thresholded);  // The stream after color filtering

    if (flag)
    {
      cv::imshow("F1", thresholded_hsv[0]);              // individual filters
      cv::imshow("F2", thresholded_hsv[1]);
      cv::imshow("F3", thresholded_hsv[2]);
    }

    if ((cvWaitKey(10) & 255) == 27)
      break;

    if (IP)
    {
      // find contours
      std::vector<std::vector<cv::Point> > contours;
      cv::Mat thresholded_Mat = thresholded;
      findContours(thresholded_Mat, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);  // Find the contours in the image
      double largest_area = 0, largest_contour_index = 0;

      if (contours.empty())
      {
        array.data.push_back(0);
        array.data.push_back(0);

        pub.publish(array);
        ros::spinOnce();
        // If ESC key pressed, Key=0x10001B under OpenCV 0.9.7(linux version),
        // remove higher bits using AND operator
        if ((cvWaitKey(10) & 255) == 27)
          break;
        continue;
      }

      for (int i = 0; i < contours.size(); i++)  // iterate through each contour.
      {
        double a = contourArea(contours[i], false);  //  Find the area of contour
        if (a > largest_area)
        {
          largest_area = a;
          largest_contour_index = i;  // Store the index of largest contour
        }
      }
      // Convex HULL
      std::vector<std::vector<cv::Point> > hull(contours.size());
      convexHull(cv::Mat(contours[largest_contour_index]), hull[largest_contour_index], false);

      cv::Mat Drawing(thresholded_Mat.rows, thresholded_Mat.cols, CV_8UC1, cv::Scalar::all(0));
      std::vector<cv::Vec4i> hierarchy;
      cv::Scalar color(255, 255, 255);

      std::vector<cv::Rect> boundRect(1);

      boundRect[0] = boundingRect(cv::Mat(contours[largest_contour_index]));

      rectangle(Drawing, boundRect[0].tl(), boundRect[0].br(), color, 2, 8, 0);

      cv::Point center;
      center.x = ((boundRect[0].br()).x + (boundRect[0].tl()).x) / 2;
      center.y = ((boundRect[0].tl()).y + (boundRect[0].br()).y) / 2;

      drawContours(Drawing, contours, largest_contour_index, color, 2, 8, hierarchy);

      cv::Mat frame_mat = frame;
      cv::Point2f screen_center;
      screen_center.x = 320;  // size of my screen
      screen_center.y = 240;

      circle(frame_mat, center, 5, cv::Scalar(0, 250, 0), -1, 8, 1);
      rectangle(frame_mat, boundRect[0].tl(), boundRect[0].br(), color, 2, 8, 0);
      circle(frame_mat, screen_center, 4, cv::Scalar(150, 150, 150), -1, 8, 0);            // center of screen

            cv::imshow("Contours", Drawing);
      cv::imshow("RealPic", frame_mat);

      w = (boundRect[0].br()).x;
      x = (boundRect[0].br()).y;
      y = (boundRect[0].tl()).y;
      z = (boundRect[0].tl()).x;
      if (w == (frame.rows)-1 || x == (frame.cols)-1 || y == 1 || z == 1)
      {
        std::cout << "hitting the wall\n";
        if ( y == 1)
           array.data.push_back(-1);  //  hits top
        if ( z == 1)
          array.data.push_back(-2);  //  hits left
        if ( w == frame.rows-1)
          array.data.push_back(-3);  //  hits bottom
        if ( x == frame.cols-1)
          array.data.push_back(-4);  //  hits right
        ros::spinOnce();
        continue;
      }
      std::cout << w << " " << x << " " << y << " " << z << "\n" << frame.cols << "  frame  " << frame.rows << "\n";

      array.data.push_back((320 - center.x));
      array.data.push_back(-(240 - center.y));
      pub.publish(array);



      ros::spinOnce();
      // loop_rate.sleep();

      // If ESC key pressed, Key=0x10001B under OpenCV 0.9.7(linux version),
      // remove higher bits using AND operator
      if ((cvWaitKey(10) & 255) == 27)
        break;
    }
    else
    {
       std::cout << "waiting\n";
       ros::spinOnce();
    }
  }
  output_cap.release();
  return 0;
}
