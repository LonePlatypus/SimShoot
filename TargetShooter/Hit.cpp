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

        cv::Vec2i hit(x, y);
        ((Hit*)userdata)->Hits.push_back(hit);
    }
}

bool Hit::getDetected()
{
    return this->detected;
}

void Hit::setDetected(bool state)
{
    this->detected = state;
}

int Hit::getHitX()
{
    return this->xHit;
}

int Hit::getHitY()
{
    return this->yHit;
}

void Hit::setHitX(int x)
{
    this->xHit = x;
}

void Hit::setHitY(int y)
{
    this->yHit = y;
}

std::vector<cv::Vec2i> Hit::getHits()
{
    return Hits;
}

//TODO
/*
int inputCamera(Camera* camera)
{
    return 0;
}
*/

//Recuperation de donnees souris
int Hit::inputMouse()
{
    cv::setMouseCallback(SHOOTING_NAME,CallBackMouse, this);
    return 0;
}

