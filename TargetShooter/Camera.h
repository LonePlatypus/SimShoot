#pragma once

#include <opencv2/core.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/core/utility.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include "aruco_nano.h"


cv::Vec2f lineIntersection(cv::Point2f A_start , cv::Point2f A_stop , cv::Point2f B_start , cv::Point2f B_stop);

std::vector<cv::Point> linearRegression(std::vector<cv::Point> points);

class TestCamPerso
{
public:
    TestCamPerso();

    bool getFrame(cv::Mat& rfFrame);


    cv::VideoCapture    cap;
};

class CaptureVideo
{
public:
    static CaptureVideo& getInstance();
    static void delInstance();

    const cv::VideoCapture& cap() const;
    cv::VideoCapture& cap();

private:
    CaptureVideo();
    virtual ~CaptureVideo();
    static CaptureVideo* instance_;

    cv::VideoCapture* cap_;
};


class Camera
{

private :
	
	int deviceID;
	std::string path_image;

    //corners of the displyed game
	cv::Vec2f topLeft;
	cv::Vec2f topRight;
	cv::Vec2f botLeft;
	cv::Vec2f botRight;

    cv::Vec4d a;
    cv::Vec4d b;

    //detected corners
	std::vector<cv::Point2f> corners;

    std::vector<cv::Point>point;

	bool calibrated;

    int cam_width;
    int cam_height;

    cv::Mat cam_matrix;
    cv::Mat cam_dist;
	
public :

	Camera(int id, int w, int h);
	Camera();

    void DisplayCamera();

	static void CallBackMouseCorner(int event, int x, int y, int flags, void* userdata);

    int getCameraIntrinsicparameters();
	int getTransformCamScreenSimple();
    int getTransformCamScreenAutomatic();
    int getTransformCamScreenAutomaticArUco();

    void findCorners( std::vector<cv::Point> leftBatch, std::vector<cv::Point> botBatch, std::vector<cv::Point> rightBatch, std::vector<cv::Point> topBatch);

    cv::Vec2f computeScreenToGame(cv::Vec2f point);

	int getWidth();
	int getHeight();

    int getCamWidth();
    int getCamHeight();

    std::vector<cv::Point> getPoints();

	void setWidth(int width_);
	void setHeight(int height_);

	bool getCalibrated();

private :

	int width;
	int height;

};

