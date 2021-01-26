#pragma once
#include <string>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/core.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"

#define SHOOTING_NAME "Shooting screen"

class Hit
{
private :

	bool detected;
	
	std::vector<cv::Vec2i> Hits;

	int xHit;
	int yHit;

    cv::VideoCapture cap;

	std::string webcam_path;

public :


	Hit();

	int inputMouse();

	static void CallBackMouse(int event, int x, int y, int flags, void* userdata);

	bool getDetected();
	void setDetected(bool state);

	int getHitX();
	int getHitY();
	void setHitX(int x);
	void setHitY(int y);

    void startVideoCap(int deviceID);
    int inputCamera();

	std::vector<cv::Vec2i> getHits();

};

