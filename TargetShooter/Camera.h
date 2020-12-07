#include <opencv2/core.hpp>
#include <vector>
#include "aruco_nano.h"
#include <string>
#include "Display.h"
#include <iostream>


#pragma once
class Camera
{
public:
	
	std::string path_camera;
	std::string path_image;

	cv::Mat markerImg;
	
	Camera(std::string pathToMarker);

	int getTransformCamScreen(Display display);
	int getTransformCamScreenSimple(Display display);

	int setCameraCalibration(std::string path);

private :


};

