#include "Camera.h"


Camera::Camera(std::string pathToMarker)
{
	double ratio = 0.5;
	cv::Mat img = cv::imread(pathToMarker, cv::IMREAD_COLOR);
	resize(img, img, cv::Size(), ratio, ratio);
	markerImg = img;

	topLeft[0] = 0.0;
	topLeft[1] = 0.0;

	topRight[0] = 0.0;
	topRight[1] = 0.0;

	botLeft[0] = 0.0;
	botLeft[1] = 0.0;

	botRight[0] = 0.0;
	botRight[1] = 0.0;
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

	width = display.display_width;
	height = display.display_height;

	//On construit l'image verte
	cv::Mat img;
	img = cv::Mat::zeros(cv::Size(width, height), CV_8UC3);
	img = cv::Scalar(0, 255, 0);

	//cv::imshow("Calibration", img);
	//cv::waitKey(10000);


	//TODO récupérer l'image
	cv::Mat src = cv::imread(cv::samples::findFile("./img/screen.jpg"), cv::IMREAD_COLOR);

	bool settingsOk = false;
	cv::namedWindow("Segmentation settings", cv::WINDOW_NORMAL);
	//Create trackbar
	int iSliderValue1 = 0;
	cv::createTrackbar("Rmin", "Segmentation settings", &iSliderValue1, 255);
	//Create trackbar
	int iSliderValue2 = 193;
	cv::createTrackbar("Gmin", "Segmentation settings", &iSliderValue2, 255);
	//Create trackbar
	int iSliderValue3 = 0;
	cv::createTrackbar("Bmin", "Segmentation settings", &iSliderValue3, 255);
	//Create trackbar
	int iSliderValue4 = 145;
	cv::createTrackbar("Rmax", "Segmentation settings", &iSliderValue4, 255);
	//Create trackbar
	int iSliderValue5 = 255;
	cv::createTrackbar("Gmax", "Segmentation settings", &iSliderValue5, 255);
	//Create trackbar
	int iSliderValue6 = 150;
	cv::createTrackbar("Bmax", "Segmentation settings", &iSliderValue6, 255);
	//Create trackbar
	int iSliderValue7 = 0;
	cv::createTrackbar("Exit", "Segmentation settings", &iSliderValue7, 1);

	std::vector<cv::Point2f> corners;

	while (!settingsOk)
	{
		if (iSliderValue7 == 1 && corners.size() == 4)
		{
			settingsOk = true;
		}

		cv::Mat img_screen = src.clone();
		cv::imshow("Screen pic", img_screen);
		cv::waitKey(50);

		cv::inRange(src, cv::Scalar(iSliderValue1, iSliderValue2, iSliderValue3), cv::Scalar(iSliderValue4, iSliderValue5, iSliderValue6), img_screen);
		cv::Mat back = cv::Mat::zeros(cv::Size(400, 2), CV_8UC3);
		imshow("Segmentation settings", back);
		cv::imshow("Screen segmented", img_screen);
		cv::waitKey(50);

		cv::Canny(img_screen, img_screen, 0.5, 50, 3);
		cv::imshow("Screen canny", img_screen);
		cv::waitKey(50);

		cv::goodFeaturesToTrack(img_screen, corners, 4, 0.1, 10);

		cv::Mat img_corners = src.clone();
		for (int i = 0; i < corners.size(); i++)
		{
			cv::Point2i pt = corners[i];
			cv::circle(img_corners, pt, 5, cv::Scalar(255, 0, 0));
		}
		cv::imshow("Screen corners", img_corners);
		cv::waitKey(50);
	}
	cv::destroyWindow("Screen pic");
	cv::destroyWindow("Segmentation settings");
	cv::destroyWindow("Screen segmented");
	cv::destroyWindow("Screen canny");
	cv::destroyWindow("Screen corners");


	if (corners.size() < 4)
	{
		return -1;
	}
	//On cherche la position des coins
	//en premier le top left -> on récupère les deux plus hauts et celui le plus à gauche est sélectionné
	cv::Point2i pt1 = corners[0];
	int pose1 = 0;
	cv::Point2i pt2 = corners[1];
	int pose2 = 1;
	for (int i = 2; i < corners.size(); i++)
	{
		cv::Point2i pt = corners[i];

		if (pt.y < pt1.y)
		{	
			if (pt1.y < pt2.y)
			{
				pt2 = pt1;
				pose2 = pose1;
			}

			pt1 = pt;
			pose1 = i;

		}
		else if (pt.y < pt2.y)
		{
			pt2 = pt;
			pose2 = i;
		}
	}

	if (pt1.x < pt2.x)
	{
		topLeft[0] = pt1.x;
		topLeft[1] = pt1.y;
		cv::circle(src, pt1, 7, cv::Scalar(255, 0, 0),3);
		cv::imshow("Corner", src);
		cv::waitKey(1000);
		topRight[0] = pt2.x;
		topRight[1] = pt2.y;
		cv::circle(src, pt2, 7, cv::Scalar(0, 0, 255),3);
		cv::imshow("Corner", src);
		cv::waitKey(1000);
	}
	else
	{
		topLeft[0] = pt2.x;
		topLeft[1] = pt2.y;
		cv::circle(src, pt2, 7, cv::Scalar(255, 0, 0),3);
		cv::imshow("Corner", src);
		cv::waitKey(2000);
		topRight[0] = pt1.x;
		topRight[1] = pt1.y;
		cv::circle(src, pt1, 7, cv::Scalar(0, 0, 255),3);
		cv::imshow("Corner", src);
		cv::waitKey(1000);
	}

	//remove corners from vector with indexes
	if (pose1 < pose2)
	{
		corners.erase(corners.begin() + pose2);
		corners.erase(corners.begin() + pose1);
	}
	else
	{
		corners.erase(corners.begin() + pose1);
		corners.erase(corners.begin() + pose2);
	}

	pt1 = corners[0];
	pt2 = corners[1];

	//on ajoute les deux corners du bas
	if (pt1.x < pt2.x)
	{
		botLeft[0] = pt1.x;
		botLeft[1] = pt1.y;
		cv::circle(src, pt1, 7, cv::Scalar(255, 0, 255),3);
		cv::imshow("Corner", src);
		cv::waitKey(1000);
		botRight[0] = pt2.x;
		botRight[1] = pt2.y;
		cv::circle(src, pt2, 7, cv::Scalar(0, 255, 255),3);
		cv::imshow("Corner", src);
		cv::waitKey(1000);
	}
	else
	{
		botLeft[0] = pt2.x;
		botLeft[1] = pt2.y;
		cv::circle(src, pt2, 7, cv::Scalar(255, 0,255),3);
		cv::imshow("Corner", src);
		cv::waitKey(2000);
		botRight[0] = pt1.x;
		botRight[1] = pt1.y;
		cv::circle(src, pt1, 7, cv::Scalar(0, 255, 255),3);
		cv::imshow("Corner", src);

	}
	cv::waitKey(2000);
	cv::destroyWindow("Corner");


/*https://www.particleincell.com/2012/quad-interpolation */
	//dans l'image caméra -> coordonnées P(x,y)
	//dans le carré unité -> coordonnées P'(l,m)

	// x = a(0) + a(1)*l + a(2)*m + a(3)*l*m
	// y = b(0) + b(1)*l + b(2)*m + b(3)*l*m


	/*

	|x1|     |1 0 0 0|   |a0|
	|x2|  =  |1 1 0 0| * |a1|
	|x3|     |1 1 1 1|   |a2|
	|x4|     |1 0 1 0|   |a3|

	donc

	|a1|     | 1  0  0  0|   |x0|
	|a2|  =  |-1  1  0  0| * |x1|
	|a3|     |-1  0  0  1|   |x2|
	|a4|     | 1 -1  1 -1|   |x3|

	-> solve

	a1 = x0
	a2 = x1 - x0
	a3 = x3 - x0
	a4 = x0 - x1 + x2 - x3

	*/

	a[0] = topLeft[0];
	a[1] = botLeft[0] - topLeft[0];
	a[2] = botRight[0] - topLeft[0];
	a[3] = topLeft[0] - botLeft[0] + topRight[0] - botRight[0];

	b[0] = topLeft[1];
	b[1] = botLeft[1] - topLeft[1];
	b[2] = botRight[1] - topLeft[1];
	b[3] = topLeft[1] - botLeft[1] + topRight[1] - botRight[1];


	return 0;
}

int Camera::setCameraCalibration(std::string path)
{
	return 0;
}


cv::Vec2f Camera::computeScreenToGame(cv::Vec2f point)
{

	/*https://www.particleincell.com/2012/quad-interpolation */
		//dans l'image caméra -> coordonnées P(x,y)
		//dans le carré unité -> coordonnées P'(u,v)

		// x = a(0) + a(1)*u + a(2)*v + a(3)*u*v
		// y = b(0) + b(1)*u + b(2)*v + b(3)*u*v


	float x = point[0];
	float y = point[1];
	float aa = a[3] * b[2] - a[2] * b[3];
	float bb = a[3] * b[0] - a[0] * b[3] + a[1] * b[2] - a[2] * b[1] + x * b[3] - y * a[3];
	float cc = a[1] * b[0] - a[0] * b[1] + x * b[1] - y * a[1];

	float det = std::sqrt(bb*bb-4*aa*cc);
	float u = (-bb + det) / (2 * aa);
	float v = (x - a[0] - a[2] * u) / (a[1] + a[3] * u);

	cv::Vec2f ret;
	ret[0] = u*(float)width;
	ret[1] = v*(float)height;

	return ret;

}