#pragma once
#include <time.h>
#include <iostream>
#include "Display.h"
#include <random>
#include "Record.h"

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
private :

	int state;
	bool up;

	double up_time;
	double down_time;

	clock_t t;

	int nbCible;
	int nbHits;
	int targetId;

public :
  Random rnd;

	game1(settings *settings, int target);

	int countDownScreen(Display *display, int time);

    int update(Display *display, Record *record);
	int start(settings* settings, int target);
	int stop(Display *display);
	int reset();

	void setNbCible(int nb);
	int getNbCible();

	void setState(bool state_);
	bool getState();

};

