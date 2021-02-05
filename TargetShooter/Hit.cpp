#include "Hit.h"

//#define DEBUG_time
//#define DEBUG_frame
//#define DEBUG_frame_rec

Hit::Hit()
{
    detected = false;
    xHit = -1;
    yHit = -1;

    camOpen = false;

    NbHit = 0;
    ignoreFrame = false;

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

void Hit::setIgnoreFrame(bool value)
{
    this->ignoreFrame = value;
}

bool Hit::getIgnoreFrame()
{
    return this->ignoreFrame;
}

int Hit::inputCamera(Camera *camera)
{

#ifdef DEBUG_time
    clock_t timeA;
    clock_t timeB;
    double elapsedA=0.0;
#endif

    if (CaptureVideo::getInstance().cap().isOpened())
    {

#ifdef DEBUG_time
        timeA= clock();
#endif

        bool read = CaptureVideo::getInstance().cap().read(laser);

        if(!ignoreFrame)
        {
            bool empty = laser.empty();
            if(read && !empty)
            {

#ifdef DEBUG_time
                timeB = clock();
                elapsedA = (double)(timeB-timeA) / CLOCKS_PER_SEC;
                std::cout <<"elapsed get image : "<<elapsedA<<std::endl;

                timeA= clock();
#endif
                extractChannel(laser, red,2);
#ifdef DEBUG_time
                timeB = clock();
                elapsedA = (double)(timeB-timeA) / CLOCKS_PER_SEC;
                std::cout <<"elapsed channel split : "<<elapsedA<<std::endl;

                timeA= clock();
#endif
                threshold(red,tresh,253,255,cv::THRESH_BINARY);
#ifdef DEBUG_time
                timeB = clock();
                elapsedA = (double)(timeB-timeA) / CLOCKS_PER_SEC;
                std::cout <<"elapsed treshold : "<<elapsedA<<std::endl;

                timeA= clock();
#endif
                int erosion_size = 1;
                element = getStructuringElement( cv::MORPH_ELLIPSE ,
                                                 cv::Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                                 cv::Point( erosion_size, erosion_size ) );
                cv::erode( tresh, erod, element);

#ifdef DEBUG_time
                timeB = clock();
                elapsedA = (double)(timeB-timeA) / CLOCKS_PER_SEC;
                std::cout <<"elapsed erosion : "<<elapsedA<<std::endl;

                timeA= clock();
#endif
                cv::dilate( erod, dil, element);
#ifdef DEBUG_time
                timeB = clock();
                elapsedA = (double)(timeB-timeA) / CLOCKS_PER_SEC;
                std::cout <<"elapsed dilatation : "<<elapsedA<<std::endl;

                timeA= clock();
#endif
                cv::Moments moment;
                moment = cv::moments(dil, true);
#ifdef DEBUG_time
                timeB = clock();
                elapsedA = (double)(timeB-timeA) / CLOCKS_PER_SEC;
                std::cout <<"elapsed moments : "<<elapsedA<<std::endl;
#endif
#ifdef DEBUG_frame
                cv::imshow("debug", laser);
                cv::waitKey(5);
#endif

                if(moment.m00 > 50 && moment.m00 < 400)
                {
                    int cX = int(moment.m10/moment.m00);
                    int cY = int(moment.m01/moment.m00);
                    cv::Vec2f hit(cX, cY);

                    cv::Vec2f hit_screen = camera->computeScreenToGame(hit);

                    this->xHit = hit_screen(0);
                    this->yHit = hit_screen(1);
                    this->Hits.push_back(hit_screen);
                    this->detected = true;
                    this->ignoreFrame = true;

#ifdef DEBUG_frame_rec
                    cv::Point2i pt;
                    pt.x = hit(0);
                    pt.y = hit(1);
                    cv::circle(laser, pt, 6 , cv::Scalar(255,0,0),2);

                    std::string name = "img_" + std::to_string(NbHit)+".jpg";
                    NbHit++;
                    cv::imwrite(name, laser);
                    cv::imshow("hit_img", laser);
                    cv::waitKey(1000);
#endif

                }

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
            ignoreFrame = false;
        }
    }
    else
    {
        std::cerr << "ERROR! Unable to open camera\n";
        return -1;
    }
}


//Recuperation de donnees vidéo
void Hit::startVideoCap()
{
    // check if we succeeded
    if (!CaptureVideo::getInstance().cap().isOpened())
    {
        std::cerr << "ERROR! Unable to open camera\n";
    }
    else
    {
        //cap.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
        //cap.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);
        camOpen = true;
    }


}

//Recuperation de donnees souris
int Hit::inputMouse()
{
    cv::setMouseCallback(SHOOTING_NAME,CallBackMouse, this);
    return 0;
}
