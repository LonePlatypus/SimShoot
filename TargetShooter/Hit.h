#pragma once
#include <string>
#include <iostream>
#include "opencv2/highgui/highgui.hpp"

#define SHOOTING_NAME "Shooting screen"

class Hit
{
private :

	bool detected;
	int xHit;
	int yHit;

	std::string webcam_path;

public :


	Hit();

	int inputCamera(std::string path);
	int inputMouse();

	static void CallBackMouse(int event, int x, int y, int flags, void* userdata);

	bool getDetected();
	void setDetected(bool state);

	int getHitX();
	int getHitY();
	void setHitX(int x);
	void setHitY(int y);

};

