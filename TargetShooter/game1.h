#pragma once
#include <time.h>
#include <iostream>
#include "Display.h"
#include <random>

using namespace std;


class game1
{
public :

	int state;
	bool up;

	double up_time;
	double down_time;

	clock_t t;

	int nbCible;
	int nbHits;
	int targetId;

	game1(settings *settings, int target);

	int countDownScreen(Display *display, int time);

	int update(Display *display);
	int start(const settings* settings, int target);
	int stop(Display *display);
	int reset();

};

