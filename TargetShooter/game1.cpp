#include "game1.h"

random_device rd;
mt19937 gen(rd());
int width = (int)GetSystemMetrics(SM_CXSCREEN);
int height = (int)GetSystemMetrics(SM_CYSCREEN);
uniform_int_distribution<> randX(0, width);
uniform_int_distribution<> randY(0, height);


game1::game1(settings *settings, int target)
{
	state = 0;
	up = false;
	nbCible = settings->nbCibles;
	nbHits = 0;
	up_time = settings->up_time;
	down_time = settings->down_time;
	targetId = target;

	t = clock();
}

int game1::countDownScreen(Display *display, int time)
{
	display->resetDisplayedbackground();
	for(int i = time; i > 0; i--)
	{
		std::string text;
		text = "Starting in ";
		text = text + to_string(i);
		text = text +"s";
		cv::putText(display->displayed_background, text, cv::Point(display->display_width / 4, display->display_height / 2), cv::FONT_HERSHEY_SIMPLEX, 5, cv::Scalar(0, 0, 255, 255), 4, 16);
		display->display(time * 1000);
		display->resetDisplayedbackground();
	}
	
	return 0;
}

int game1::update(Display *display)
{

	clock_t time = clock() - t;
	double elapsed = ((double)time) / CLOCKS_PER_SEC;

	//si la cible doit être affichée
	if (up)
	{
		//si elle doit encore être affiché, on continue ...
		if (elapsed < up_time)
		{
			display->display(1);
		}
		//sinon on met l'écran "neutre" et on lance le chrono pour le rechargement
		else
		{
			//TODO compter les tirs + évaluer les tirs aux bon endroit


			up = false;
			t = clock();
			nbCible--;
			display->resetDisplayedbackground();
		}
	}
	//la cible doit être cachée
	else
	{
		//si elle doit encore l'être, on continue
		if (elapsed < down_time)
		{
			display->display(1);
		}
		//sinon on génère la nouvelle position de la cible sur l'écran
		else
		{
			int rX = randX(gen);
			int rY = randY(gen);

			while (display->addTarget(rX, rY, targetId) == -1)
			{
				rX = randX(gen);
				rY = randY(gen);
			}
			up = true;
			t = clock();
			display->display(1);
		}
	}

	return 0;
}

int game1::start(const settings *settings, int target)
{
	state = 1;

	up_time = settings->up_time;
	down_time = settings->down_time;
	nbCible = settings->nbCibles;
	targetId = target;
	up = true;
	return 0;
}

int game1::stop(Display *display)
{
	display->resetDisplayedbackground();
	std::string text;
	text = "Stop";
	cv::putText(display->displayed_background, text, cv::Point(display->display_width / 2, display->display_height / 2), cv::FONT_HERSHEY_SIMPLEX, 5, cv::Scalar(0, 0, 255, 255), 4, 16);
	display->display(1000);
	state = 0;
	display->resetDisplayedbackground();
	return 0;
}

int game1::reset()
{
	nbCible = 10;
	nbHits = 0;
	return 0;
}
