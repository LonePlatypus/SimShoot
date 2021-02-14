#pragma once
#include <string>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/core.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"

#include <time.h>
#include "Camera.h"

#define SHOOTING_NAME "Shooting screen"

class Hit
{
private :

	bool detected;
	
	std::vector<cv::Vec2i> Hits;

    std::vector<cv::Vec2i> Cam_Hits;
    int cam_xHit;
    int cam_yHit;

	int xHit;
	int yHit;

    cv::VideoCapture cap;

	std::string webcam_path;
    bool camOpen;

    cv::Mat laser;
    cv::Mat red;
    cv::Mat tresh;
    cv::Mat erod;
    cv::Mat dil;
    cv::Mat element;

    int NbHit;

    bool ignoreFrame;

public :


	Hit();

	int inputMouse();

    static void CallBackMouse(int event, int x, int y, int flags,void* userdata);

    bool getCamOpen();

	bool getDetected();
	void setDetected(bool state);

	int getHitX();
	int getHitY();
	void setHitX(int x);
	void setHitY(int y);

    void setIgnoreFrame(bool value);
    bool getIgnoreFrame();

    void startVideoCap();
    int inputCamera(Camera *camera, double gamma);

	std::vector<cv::Vec2i> getHits();

};

