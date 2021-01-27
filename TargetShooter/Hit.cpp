#include "Hit.h"
#include <time.h>

Hit::Hit()
{
    detected = false;
    xHit = -1;
    yHit = -1;

    camOpen = false;
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

bool Hit::getCamOpen()
{
    return this->camOpen;
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


int Hit::inputCamera()
{
    clock_t timeA;
    clock_t timeB;
    double elapsedA=0.0;

    //cv::Mat laser = cv::imread("img/img30.jpg");

    timeA= clock();
    int apiID = cv::CAP_DSHOW ;
    cap.open(0, apiID);
    timeB = clock();
    elapsedA = (double)(timeB-timeA) / CLOCKS_PER_SEC;
    std::cout <<"elapsed set image size : "<<elapsedA<<std::endl;


    if (cap.isOpened())
    {

        timeA= clock();

        //cap >> laser;

        bool read = cap.read(laser);
        bool empty = laser.empty();
        if(read && !empty)
        //if(cap.grab() && cap.retrieve(laser) && !laser.empty())
        {

        timeB = clock();
        elapsedA = (double)(timeB-timeA) / CLOCKS_PER_SEC;
        std::cout <<"elapsed get image : "<<elapsedA<<std::endl;

        timeA= clock();
        extractChannel(laser, red,2);
        timeB = clock();
        elapsedA = (double)(timeB-timeA) / CLOCKS_PER_SEC;
        std::cout <<"elapsed channel : "<<elapsedA<<std::endl;

        timeA= clock();
        threshold(red,tresh,240,255,cv::THRESH_BINARY);
        timeB = clock();
        elapsedA = (double)(timeB-timeA) / CLOCKS_PER_SEC;
        std::cout <<"elapsed tresh : "<<elapsedA<<std::endl;

        timeA= clock();
        int erosion_size = 1;
        element = getStructuringElement( cv::MORPH_ELLIPSE ,
                                         cv::Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                         cv::Point( erosion_size, erosion_size ) );
        cv::erode( tresh, erod, element);
        timeB = clock();
        elapsedA = (double)(timeB-timeA) / CLOCKS_PER_SEC;
        std::cout <<"elapsed erode : "<<elapsedA<<std::endl;

        timeA= clock();
        cv::dilate( erod, dil, element);
        timeB = clock();
        elapsedA = (double)(timeB-timeA) / CLOCKS_PER_SEC;
        std::cout <<"elapsed dil : "<<elapsedA<<std::endl;

        timeA= clock();
        cv::Moments moment;
        moment = cv::moments(dil, true);

        timeB = clock();
        elapsedA = (double)(timeB-timeA) / CLOCKS_PER_SEC;
        std::cout <<"elapsed mom : "<<elapsedA<<std::endl;

        if(moment.m00 > 10 && moment.m00 > 1000)
        {
            int cX = int(moment.m10/moment.m00);
            int cY = int(moment.m01/moment.m00);
            cv::Vec2i hit(cX, cY);
            this->xHit = cX;
            this->yHit = cY;
            this->Hits.push_back(hit);
            this->detected = true;
        }

        //std::cout << "10 : "<< moment.m10 << "   |00 : "<< moment.m00 << "   |01 : "<<moment.m01 <<std::endl;

        return 0;
        }
        else
        {
            std::cerr << "ERROR! no frame\n";
            return -1;
        }
    }
    else
    {
        std::cerr << "ERROR! Unable to open camera\n";
        return -1;
    }
}


//Recuperation de donnees vidéo
void Hit::startVideoCap(int deviceID)
{
    int apiID = cv::CAP_DSHOW ;
    cap.open(deviceID, apiID);
    // check if we succeeded
    if (!cap.isOpened())
    {
        std::cerr << "ERROR! Unable to open camera\n";
    }
    else
    {
        //cap.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
        //cap.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);
        //cap.release();
        camOpen = true;
    }


}

//Recuperation de donnees souris
int Hit::inputMouse()
{
    cv::setMouseCallback(SHOOTING_NAME,CallBackMouse, this);
    return 0;
}

