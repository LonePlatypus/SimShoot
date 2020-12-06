#pragma once
#include <opencv2/opencv.hpp>

#define SETTINGS_NAME "Settings"

class settings
{
public:

    int settings_width;
    int settings_height;
    cv::Mat settings_frame;


    bool Target_ISPC;
    bool Target_round;
    double up_time;
    double down_time;

    bool stop;
    bool start;
    bool exit;

    int nbCibles;


    settings();

    int displaySettings();

};

