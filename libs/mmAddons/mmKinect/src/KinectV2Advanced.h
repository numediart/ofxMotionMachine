#ifndef KINECTADV_H
#define KINECTADV_H

#include "KinectV2.h"
#include "opencv/cv.h"
#include <math.h> 
#include <iostream>

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/common/io.h>

struct Point3D{
	float X;
	float Y;
	float Z;
};

struct Region{
	float X;
	float Y;
	float W;
	float H;
};


class KinectV2Advanced
{

public:

	KinectV2Advanced();

	~KinectV2Advanced();

	bool isCameraConnected();

	//update
	bool update();
	bool getImageMat(cv::Mat& image);
	bool getColorPointCloud(pcl::PointCloud<pcl::PointXYZRGBA>::Ptr &cloud);
	bool getColorPointCloudLowRes(pcl::PointCloud<pcl::PointXYZRGBA>::Ptr &cloud);
	bool getDepthPointCloud(pcl::PointCloud<pcl::PointXYZRGBA>::Ptr &cloud);

private:

	// CAMERA
	KINECTV2::KinectV2* kinectcam;
	bool camConnected;

	// DATA

	cv::Mat kinect_color_image;

};

#endif