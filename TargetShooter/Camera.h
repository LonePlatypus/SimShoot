#pragma once

#include <opencv2/core.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/core/utility.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>

class TestCamPerso
{
public:
    TestCamPerso();

    bool getFrame(cv::Mat& rfFrame);


    cv::VideoCapture    cap;
};


class Camera
{
private :
	
	int deviceID;
	std::string path_image;

	cv::Vec2f topLeft;
	cv::Vec2f topRight;
	cv::Vec2f botLeft;
	cv::Vec2f botRight;

	cv::Vec4f a;
	cv::Vec4f b;

	std::vector<cv::Point2f> corners;

	bool calibrated;
	
public :

	Camera(int id, int w, int h);
	Camera();

    void DisplayCamera();

	static void CallBackMouseCorner(int event, int x, int y, int flags, void* userdata);

	int getTransformCamScreenSimple();

	cv::Vec2f computeScreenToGame(cv::Vec2f point);

	int getWidth();
	int getHeight();

	void setWidth(int width_);
	void setHeight(int height_);

	bool getCalibrated();

private :

	int width;
	int height;

};

