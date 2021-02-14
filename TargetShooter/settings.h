#include <opencv2/opencv.hpp>
#include "Camera.h"

#define SETTINGS_NAME "Settings"

class settings
{

private :

    cv::Mat settings_frame;

    int settings_width;
    int settings_height;

    bool Target_ISPC;
    bool Target_round;

    bool stop;
    bool start;
    bool exit;

    bool useMouse;
    bool useCamera;

    double up_time;
    double down_time;
    int nbCibles;

    double gamma;

public:

    settings();

    int displaySettings(Camera *camera, bool camOpen);

    void setWidth(int width);
    void setHeight(int height);
    int getWidth();
    int getHeight();

    void setTargetISPC(bool state);
    void setTargetRound(bool state);
    bool getTargetISPC();
    bool getTargetRound();

    int getInput();

    void setStop(bool state);
    void setStart(bool state);
    void setExit(bool state);
    bool getStop();
    bool getStart();
    bool getExit();

    double getGamma();

    void setUpTime(double time);
    void setDownTime(double time);
    void setNbCible(int nb);
    double getUpTime();
    double getDownTime();
    int getNbCibles();

};

