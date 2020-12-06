#pragma once
#include <string>
#include <iostream>
#include <Windows.h>
#include "opencv2/highgui/highgui.hpp"

#define SHOOTING_NAME "Shooting screen"

class Hit
{
public :

	bool detected;
	int xHit;
	int yHit;

	std::string webcam_path;

	Hit();

	int inputCamera(std::string path);
	int inputMouse();

	static void CallBackMouse(int event, int x, int y, int flags, void* userdata);

	
};

