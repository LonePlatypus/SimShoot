#pragma once
#include <time.h>
#include <iostream>
#include "Display.h"
#include <random>

using namespace std;

struct Random
{
  random_device rd;
  mt19937 gen;
  uniform_int_distribution<> randX;
  uniform_int_distribution<> randY;

  Random();

  int rndX();
  int rndY();
};

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

  Random rnd;

	game1(settings *settings, int target);

	int countDownScreen(Display *display, int time);

	int update(Display *display);
	int start(const settings* settings, int target);
	int stop(Display *display);
	int reset();

};

