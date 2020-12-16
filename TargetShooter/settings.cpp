#include "settings.h"
#define CVUI_IMPLEMENTATION
#include "cvui.h"

settings::settings()
{
    cv::namedWindow(SETTINGS_NAME);
    cvui::init(SETTINGS_NAME);

    settings_width = 400;
    settings_height = 600;
    settings_frame = cv::Mat::zeros(cv::Size(settings_width, settings_height), CV_8UC3);


    Target_ISPC = true;
    Target_round = false;
    exit = false;
    start = false;
    stop = false;
    up_time = 3.0;
    down_time = 1.0;
    nbCibles = 10;

    useMouse = true;
    useCamera = false;

}

int settings::displaySettings(Camera *camera)
{
    cvui::window(settings_frame, 0, 0, settings_width, settings_height, "Settings");

    cvui::checkbox(settings_frame, 10, 30, "Target ISPC", &Target_ISPC);
    if (Target_ISPC == true)
    {
        Target_round = false;
    }
    cvui::checkbox(settings_frame, 210, 30, "Target round", &Target_round);
    if (Target_round == true)
    {
        Target_ISPC = false;
    }
    cvui::text(settings_frame, 10, 70, "Temps d'apparition");
    cvui::trackbar(settings_frame, 10, 90, 380, &up_time, 0.5, 7.0);

    cvui::text(settings_frame, 10, 160, "Temps de rechargement");
    cvui::trackbar(settings_frame, 10, 180, 380, &down_time, 0.1, 3.0);

    cvui::text(settings_frame, 170, 255, "Nombre de cible par session");
    cvui::counter(settings_frame, 30, 250, &nbCibles, 1,"%d");

    cvui::checkbox(settings_frame, 10, 300, "Mouse Input", &useMouse);
    cvui::checkbox(settings_frame, 210, 300, "Camera Input", &useCamera);
    if (!camera->getCalibrated())
    {
        useCamera = false;
    }

    if (cvui::button(settings_frame, 70, 460, 100, 50, "Start")) {
        // button was clicked
        start = true;
        stop = false;
    }
    if (cvui::button(settings_frame, 230, 460, 100, 50, "Stop")) {
        // button was clicked
        start = false;
        stop = true;
    }
    if (cvui::button(settings_frame, 70, 530, 100, 50, "EXIT")) {
        // button was clicked
        exit = true;
    }

    if (cvui::button(settings_frame, 230, 530, 100, 50, "Calib' camera")) {
        // button was clicked
        camera->getTransformCamScreenSimple();
    }

    cvui::imshow(SETTINGS_NAME, settings_frame);

    return 0;
}

void settings::setWidth(int width)
{
    this->settings_width = width;
}

void settings::setHeight(int height)
{
    settings_height = height;
}

int settings::getWidth()
{
    return settings_width;
}

int settings::getHeight()
{
    return settings_height;
}

void settings::setTargetISPC(bool state)
{
    Target_ISPC = state;
}

void settings::setTargetRound(bool state)
{
    Target_round = state;
}

bool settings::getTargetISPC()
{
    return Target_ISPC;
}

bool settings::getTargetRound()
{
    return Target_round;
}

int settings::getInput()
{
    if (useCamera)
    {
        return 1;
    }
    else if (useMouse)
    {
        return 2;
    }
    else
    {
        return 0;
    }
}

void settings::setStop(bool state)
{
    stop = state;
}

void settings::setStart(bool state)
{
    start = state;
}

void settings::setExit(bool state)
{
    exit = state;
}

bool settings::getStop()
{
    return stop;
}

bool settings::getStart()
{
    return start;
}

bool settings::getExit()
{
    return exit;
}

void settings::setUpTime(double time)
{
    this->up_time = time;
}

void settings::setDownTime(double time)
{
    this->down_time = time;
}

void settings::setNbCible(int nb)
{
    this->nbCibles = nb;
}

double settings::getUpTime()
{
    return up_time;
}

double settings::getDownTime()
{
    return down_time;
}

int settings::getNbCibles()
{
    return nbCibles;
}
