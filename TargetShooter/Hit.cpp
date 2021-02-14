#include "Hit.h"

//#define DEBUG_time
#define DEBUG_frame
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

int Hit::inputCamera(Camera *camera, double gamma)
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
                cv::Mat img_gamma_corrected;
                //gamma correction
                CV_Assert(gamma >= 0);
                //! [changing-contrast-brightness-gamma-correction]
                cv::Mat lookUpTable(1, 256, CV_8U);
                uchar* p = lookUpTable.ptr();
                for( int i = 0; i < 256; ++i)
                    p[i] = cv::saturate_cast<uchar>(pow(i / 255.0, gamma) * 255.0);

                cv::Mat res = red.clone();
                LUT(red, lookUpTable, res);

                //remove external from screen to avoid saturation
                cv::Mat mask = cv::Mat::zeros(res.size(), res.type());
                std::vector<cv::Point> pts = camera->getPoints();
                cv::Point corners[1][4];
                corners[0][0] = pts[0];
                corners[0][1] = pts[1];
                corners[0][2] = pts[2];
                corners[0][3] = pts[3];
                const cv::Point* corner_list[1] = { corners[0] };
                const int size = 4;
                cv::fillPoly( mask, corner_list, &size, 1, cv::Scalar( 255, 255, 255 ));
                cv::bitwise_and(res, mask, res);

                threshold(res,tresh,253,255,cv::THRESH_BINARY);
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
                cv::imshow("res", res);
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
        return 0;
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
