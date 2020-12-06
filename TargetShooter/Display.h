#pragma once

#include <opencv2/opencv.hpp>
#include <random>
#include "settings.h"
#include "Hit.h"

#define SHOOTING_NAME "Shooting screen"

class Display
{
public:
  static void GetScreenResoluton(int& rfWidth,
                                 int& rfHeight);
public:

	int display_width;
	int display_height;
	cv::Mat background;

	cv::Mat displayed_background;
	std::vector<cv::Mat> targets;

	cv::Mat hit;

	Display();
	int resetDisplayedbackground();

	int loadTargets();
	int loadPNGTarget(std::string path, double ratio);
	int loadJPGTarget(std::string path, double ratio);
	int loadHit();


	int addTarget(int X , int Y, int targetId);
	int addHit(int X, int Y);

	int display(int delay);

	int getTargetId(settings *setting);

	int updateHit(Hit *hit);

};

