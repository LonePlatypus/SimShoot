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

}

int settings::displaySettings()
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
    if (cvui::button(settings_frame, 70, 530, 260, 50, "EXIT")) {
        // button was clicked
        exit = true;
    }
    cvui::imshow(SETTINGS_NAME, settings_frame);

    return 0;
}
