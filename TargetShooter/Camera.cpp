#include "Camera.h"


Camera::Camera(std::string pathToMarker)
{
	double ratio = 0.5;
	cv::Mat img = cv::imread(pathToMarker, cv::IMREAD_COLOR);
	resize(img, img, cv::Size(), ratio, ratio);
	markerImg = img;
}

int Camera::getTransformCamScreen(Display display)
{
	//On construit l'image avec les coins "aruco"
	cv::Mat img;
	img = cv::Mat::zeros(cv::Size(display.display_width, display.display_height), CV_8UC3);
	img = cv::Scalar(255, 255, 255);

	int overlay_height = markerImg.rows;
	int overlay_width = markerImg.cols;

	int Y = (overlay_height/2);
	int X = (overlay_width/2);

	if (X - (int)(overlay_width / 2) >= 0 && Y - (int)(overlay_height / 2) >= 0 && X + (int)(overlay_width / 2) <= display.display_width && Y + (int)(overlay_height / 2) <= display.display_height)
	{
		cv::Rect center(X - overlay_width / 2, Y - overlay_height / 2, overlay_width, overlay_height);
		markerImg.copyTo(img(center));
	}

	Y = display.display_height - (overlay_height / 2)-21;
	X = (overlay_width / 2);

	if (X - (int)(overlay_width / 2) >= 0 && Y - (int)(overlay_height / 2) >= 0 && X + (int)(overlay_width / 2) <= display.display_width && Y + (int)(overlay_height / 2) <= display.display_height)
	{
		cv::Rect center(X - overlay_width / 2, Y - overlay_height / 2, overlay_width, overlay_height);
		markerImg.copyTo(img(center));
	}

	Y = display.display_height-(overlay_height / 2)-21;
	X = display.display_width - (overlay_width / 2)-1;

	if (X - (int)(overlay_width / 2) >= 0 && Y - (int)(overlay_height / 2) >= 0 && X + (int)(overlay_width / 2) <= display.display_width && Y + (int)(overlay_height / 2) <= display.display_height)
	{
		cv::Rect center(X - overlay_width / 2, Y - overlay_height / 2, overlay_width, overlay_height);
		markerImg.copyTo(img(center));
	}

	Y = (overlay_height / 2);
	X = display.display_width -(overlay_width / 2)-1;

	if (X - (int)(overlay_width / 2) >= 0 && Y - (int)(overlay_height / 2) >= 0 && X + (int)(overlay_width / 2) <= display.display_width && Y + (int)(overlay_height / 2) <= display.display_height)
	{
		cv::Rect center(X - overlay_width / 2, Y - overlay_height / 2, overlay_width, overlay_height);
		markerImg.copyTo(img(center));
	}


	//on affiche à l'écran pour faire la calibration 
	//TODO ... peut être rajouter un compte à rebours pour cacher toutes les autre fenêtres
	cv::imshow("Calibration", img);
	cv::waitKey(1000);

	//NO WORKING
	//aruconano::MarkerDetector ArDetector;
	//ArDetector.detect(img);

	//NO WORKING EITHER
	//std::vector<aruconano::Marker> markers = aruconano::MarkerDetector::detect(img);

	img = cv::Mat::zeros(cv::Size(display.display_width, display.display_height), CV_8UC3);
	img = cv::Scalar(255, 255, 255);

	//TEST detection et affichage des markers ...FAIL

	auto markers = aruconano::MarkerDetector::detect(img);
	for (const auto& m : markers)
		m.draw(img);

	cv::imshow("Calibration2", img);
	cv::waitKey(1000);
	return 0;
}

int Camera::getTransformCamScreenSimple(Display display)
{
	//On construit l'image avec les coins "aruco"
	cv::Mat img;
	img = cv::Mat::zeros(cv::Size(display.display_width, display.display_height), CV_8UC3);
	img = cv::Scalar(0, 255, 0);

	cv::imshow("Calibration", img);
	cv::waitKey(10000);
	return 0;
}

int Camera::setCameraCalibration(std::string path)
{
	return 0;
}
