#include <opencv2/core.hpp>
#include <vector>
#include "aruco_nano.h"
#include <string>
#include "Display.h"
#include <iostream>
#include <cmath>


#pragma once
class Camera
{
public:
	
	std::string path_camera;
	std::string path_image;

	cv::Mat markerImg;

	cv::Vec2f topLeft;
	cv::Vec2f topRight;
	cv::Vec2f botLeft;
	cv::Vec2f botRight;

	cv::Vec4f a;
	cv::Vec4f b;

	int width;
	int height;

	
	Camera(std::string pathToMarker);

	int getTransformCamScreen(Display display);
	int getTransformCamScreenSimple(Display display);

	int setCameraCalibration(std::string path);

	cv::Vec2f computeScreenToGame(cv::Vec2f point);

private :


};

