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


int Hit::inputCamera()
{
    //cv::Mat laser = cv::imread("img/img30.jpg");
    cv::Mat laser;
    if (!cap.isOpened())
    {
        std::cerr << "ERROR! Unable to open camera\n";
        return -1;
    }
    else
    {
        cap.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);

        cap >> laser;
        cv::Mat red;

        cv::imshow("laser",laser);
        cv::waitKey(500);
        cv::destroyWindow("laser");

        extractChannel(laser, red,2);

        cv::imshow("laser",red);
        cv::waitKey(500);
        cv::destroyWindow("laser");

        cv::Mat tresh;
        threshold(red,tresh,240,255,cv::THRESH_BINARY);

        cv::imshow("laser",tresh);
        cv::waitKey(500);
        cv::destroyWindow("laser");

        cv::Mat erod;
        int erosion_size = 1;
        cv::Mat element = getStructuringElement( cv::MORPH_ELLIPSE ,
                                                 cv::Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                                 cv::Point( erosion_size, erosion_size ) );

        cv::erode( tresh, erod, element );

        cv::imshow("laser",erod);
        cv::waitKey(500);
        cv::destroyWindow("laser");

        cv::Mat dil;
        cv::dilate( erod, dil, element );

        cv::imshow("laser",dil);
        cv::waitKey(500);
        cv::destroyWindow("laser");

        cv::Moments moment;
        moment = cv::moments(dil, true);
        if(moment.m00 != 0)
        {
            int cX = int(moment.m10/moment.m00);
            int cY = int(moment.m01/moment.m00);
            cv::Vec2i hit(cX, cY);
            this->xHit = cX;
            this->yHit = cY;
            this->Hits.push_back(hit);
            this->detected = true;
        }

        std::cout << "10 : "<< moment.m10 << "   |00 : "<< moment.m00 << "   |01 : "<<moment.m01 <<std::endl;

        cv::waitKey(60000);
        return 0;
    }
}


//Recuperation de donnees vidéo
void Hit::startVideoCap(int deviceID)
{
    int apiID = cv::CAP_ANY;      // 0 = autodetect default API
    // open selected camera using selected API
    cap.open(deviceID, apiID);
    // check if we succeeded
    if (!cap.isOpened())
    {
        std::cerr << "ERROR! Unable to open camera\n";
    }
    else
    {
        cap.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);

        /*
        cv::Mat img;
        for (int i = 0; i< 10 ; i++)
        {
            cap >> img;
            cv::imshow("debug cam",img);
            cv::waitKey(35);
        }
        cv::destroyWindow("debug cam");
        */
    }


}

//Recuperation de donnees souris
int Hit::inputMouse()
{
    cv::setMouseCallback(SHOOTING_NAME,CallBackMouse, this);
    return 0;
}

