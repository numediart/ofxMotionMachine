#include "KinectV2Advanced.h"
#include <cmath>  

KinectV2Advanced::KinectV2Advanced(){
	kinectcam = new KINECTV2::KinectV2();

	camConnected = SUCCEEDED(kinectcam->Initialize());
}

KinectV2Advanced::~KinectV2Advanced(){
	kinectcam->Finalize();

}

bool KinectV2Advanced::isCameraConnected(){
	//return kinectcam->GetIsKinectAvailable();
	return camConnected;
}

bool KinectV2Advanced::update(){
	HRESULT result = kinectcam->Update();
	bool bResult = SUCCEEDED(result);

	return bResult;
}

bool KinectV2Advanced::getImageMat(cv::Mat& image){
	this->update();
	image = cv::Mat(KINECTV2::LOCAL_ColorHeight, KINECTV2::LOCAL_ColorWidth, CV_8UC4, this->kinectcam->GetColorImageBuffer());
	return true;
}

bool KinectV2Advanced::getColorPointCloud(pcl::PointCloud<pcl::PointXYZRGBA>::Ptr &cloud){
	bool resulst = this->update();

	if (!resulst)
		return false;
	CameraSpacePoint* _camSpacePoint = kinectcam->GetColorCoordinatesInCameraSpace();
	RGBQUAD* _image = kinectcam->GetColorImageBuffer();
	std::cout << (int)_image[500 * KINECTV2::LOCAL_ColorWidth + 1000].rgbRed << std::endl;
	for (int y = 0; y < KINECTV2::LOCAL_ColorHeight; y++){
		for (int x = 0; x < KINECTV2::LOCAL_ColorWidth; x++){
			cloud->points[y * KINECTV2::LOCAL_ColorWidth + x].x = _camSpacePoint[y * KINECTV2::LOCAL_ColorWidth + x].X;
			cloud->points[y * KINECTV2::LOCAL_ColorWidth + x].y = _camSpacePoint[y * KINECTV2::LOCAL_ColorWidth + x].Y;
			cloud->points[y * KINECTV2::LOCAL_ColorWidth + x].z = _camSpacePoint[y * KINECTV2::LOCAL_ColorWidth + x].Z;

			cloud->points[y * KINECTV2::LOCAL_ColorWidth + x].r = (uint8_t)_image[y * KINECTV2::LOCAL_ColorWidth + x].rgbRed;
			cloud->points[y * KINECTV2::LOCAL_ColorWidth + x].g = (uint8_t)_image[y * KINECTV2::LOCAL_ColorWidth + x].rgbGreen;
			cloud->points[y * KINECTV2::LOCAL_ColorWidth + x].b = (uint8_t)_image[y * KINECTV2::LOCAL_ColorWidth + x].rgbBlue;
			//cloud->points[y * KINECTV2::LOCAL_ColorWidth + x].a = 1;
		}
	}

	return true;
}

bool KinectV2Advanced::getColorPointCloudLowRes(pcl::PointCloud<pcl::PointXYZRGBA>::Ptr &cloud){
	bool resulst = this->update();
	if (cloud->height != KINECTV2::LOCAL_DepthHeight || cloud->width != KINECTV2::LOCAL_DepthWidth){
		cloud.reset(new pcl::PointCloud<pcl::PointXYZRGBA>);
		cloud->height = KINECTV2::LOCAL_DepthHeight;
		cloud->width = KINECTV2::LOCAL_DepthWidth;
		cloud->resize(cloud->height * cloud->width);
	}

	if (!resulst)
		return false;
	
	CameraSpacePoint* _camSpacePoint = kinectcam->GetDepthCoordinatesInCameraSpace();
	ColorSpacePoint* _colorDepthSpacePoint = kinectcam->GetColorCoordinatesInDepthSpace();
	RGBQUAD* _image = kinectcam->GetColorImageBuffer();
	
	for (int y = 0; y < KINECTV2::LOCAL_DepthHeight; y++){
		
		for (int x = 0; x < KINECTV2::LOCAL_DepthWidth; x++){
			cloud->points[y * KINECTV2::LOCAL_DepthWidth + x].x = _camSpacePoint[y * KINECTV2::LOCAL_DepthWidth + x].X;
			cloud->points[y * KINECTV2::LOCAL_DepthWidth + x].y = _camSpacePoint[y * KINECTV2::LOCAL_DepthWidth + x].Y;
			cloud->points[y * KINECTV2::LOCAL_DepthWidth + x].z = _camSpacePoint[y * KINECTV2::LOCAL_DepthWidth + x].Z;

			ColorSpacePoint p = _colorDepthSpacePoint[y*KINECTV2::LOCAL_DepthWidth+x];
			if (p.X != -std::numeric_limits<float>::infinity() && p.Y != -std::numeric_limits<float>::infinity())
			{
				int colorX = static_cast<int>(p.X + 0.5f);
				int colorY = static_cast<int>(p.Y + 0.5f);

				if ((colorX >= 0 && colorX < KINECTV2::LOCAL_ColorWidth) && (colorY >= 0 && colorY < KINECTV2::LOCAL_ColorHeight))
				{
					int colorIndex = colorY * KINECTV2::LOCAL_ColorWidth + colorX;
					cloud->points[y * KINECTV2::LOCAL_DepthWidth + x].r = (uint8_t)_image[colorIndex].rgbRed;
					cloud->points[y * KINECTV2::LOCAL_DepthWidth + x].g = (uint8_t)_image[colorIndex].rgbGreen;
					cloud->points[y * KINECTV2::LOCAL_DepthWidth + x].b = (uint8_t)_image[colorIndex].rgbBlue;
					cloud->points[y * KINECTV2::LOCAL_DepthWidth + x].a = 255;
				}
			}
		}
	}

	return true;
}

bool KinectV2Advanced::getDepthPointCloud(pcl::PointCloud<pcl::PointXYZRGBA>::Ptr &cloud){
	bool resulst = this->update();

	if (!resulst)
		return false;
	CameraSpacePoint* _camSpacePoint = kinectcam->GetColorCoordinatesInCameraSpace();
	RGBQUAD* _image = kinectcam->GetColorImageBuffer();
	std::cout << (int)_image[500 * KINECTV2::LOCAL_ColorWidth + 1000].rgbRed << std::endl;
	for (int y = 0; y < KINECTV2::LOCAL_ColorHeight; y++){
		for (int x = 0; x < KINECTV2::LOCAL_ColorWidth; x++){
			cloud->points[y * KINECTV2::LOCAL_ColorWidth + x].x = _camSpacePoint[y * KINECTV2::LOCAL_ColorWidth + x].X;
			cloud->points[y * KINECTV2::LOCAL_ColorWidth + x].y = _camSpacePoint[y * KINECTV2::LOCAL_ColorWidth + x].Y;
			cloud->points[y * KINECTV2::LOCAL_ColorWidth + x].z = _camSpacePoint[y * KINECTV2::LOCAL_ColorWidth + x].Z;

			cloud->points[y * KINECTV2::LOCAL_ColorWidth + x].r = (uint8_t)_image[y * KINECTV2::LOCAL_ColorWidth + x].rgbRed;
			cloud->points[y * KINECTV2::LOCAL_ColorWidth + x].g = (uint8_t)_image[y * KINECTV2::LOCAL_ColorWidth + x].rgbGreen;
			cloud->points[y * KINECTV2::LOCAL_ColorWidth + x].b = (uint8_t)_image[y * KINECTV2::LOCAL_ColorWidth + x].rgbBlue;
			cloud->points[y * KINECTV2::LOCAL_ColorWidth + x].a = 255;
		}
	}

	return true;
}
