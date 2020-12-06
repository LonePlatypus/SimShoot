#include "Hit.h"

Hit::Hit()
{
    detected = false;
    xHit = -1;
    yHit = -1;
}

//Traitement des donnees souris
void Hit::CallBackMouse(int event, int x, int y, int flags, void* userdata)
{

    if (event == cv::EVENT_LBUTTONDOWN)
    {
        ((Hit*)userdata)->xHit = x;
        ((Hit*)userdata)->yHit = y;
        ((Hit*)userdata)->detected = true;
    }
}

//TODO
int Hit::inputCamera(std::string path)
{
    return 0;
}

//Recuperation de donnees souris
int Hit::inputMouse()
{
    cv::setMouseCallback(SHOOTING_NAME,CallBackMouse, this);
    return 0;
}

