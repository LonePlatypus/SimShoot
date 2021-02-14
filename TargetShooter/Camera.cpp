#include "Camera.h"
#include <cassert>

Camera::Camera()
{

    topLeft[0] = 0.0;
    topLeft[1] = 0.0;

    topRight[0] = 0.0;
    topRight[1] = 0.0;

    botLeft[0] = 0.0;
    botLeft[1] = 0.0;

    botRight[0] = 0.0;
    botRight[1] = 0.0;

    deviceID = 0;
    path_image = "./img/screen.jpg";

    setWidth(800);
    setHeight(600);

    calibrated = false;
}

Camera::Camera(int id, int w, int h)
{

    topLeft[0] = 0.0;
    topLeft[1] = 0.0;

    topRight[0] = 0.0;
    topRight[1] = 0.0;

    botLeft[0] = 0.0;
    botLeft[1] = 0.0;

    botRight[0] = 0.0;
    botRight[1] = 0.0;

    deviceID = id;
    path_image = "./img/screen.jpg";

    setWidth(w);
    setHeight(h);

    calibrated = false;
}

void Camera::DisplayCamera()
{
    cv::Mat src;
    // check if we succeeded
    if (!CaptureVideo::getInstance().cap().isOpened()) {
        std::cerr << "ERROR! Unable to open camera\n";
    }

    //cap.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
    //cap.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);
    int i = 0;
    while(CaptureVideo::getInstance().cap().isOpened())
    {
        // wait for a new frame from camera and store it into 'frame'
        //cap.read(src);
        CaptureVideo::getInstance().cap() >> src;
        // check if we succeeded
        if (src.empty()) {
            std::cerr << "ERROR! blank frame grabbed\n";
            break;
        }

        cv::imshow("Camera",src);
        //std::string name = "img"+ std::to_string(i)+".jpg";
        //cv::imwrite(name,src);
        i++;


        // Press  ESC on keyboard to exit
        char c=(char)cv::waitKey(25);
        if(c==27)
        {
            break;
        }
    }
    cv::destroyWindow("Camera");

}

void Camera::CallBackMouseCorner(int event, int x, int y, int flags, void* userdata)
{
    if (event == cv::EVENT_LBUTTONDOWN)
    {
        cv::Point2f point;

        point.x = x;
        point.y = y;

        ((Camera*)userdata)->corners.push_back(point);
    }
}

int Camera::getTransformCamScreenSimple()
{
    corners.clear();
    //On construit l'image verte
    cv::Mat img;
    img = cv::Mat::zeros(cv::Size(width, height), CV_8UC3);
    img = cv::Scalar(0, 255, 0);
    cv::imshow("Calibration", img);
    cv::waitKey(1000);

    cv::Mat src;
    if (!CaptureVideo::getInstance().cap().isOpened()) {
        std::cerr << "ERROR! Unable to open camera\n";
        return -1;
    }

    //cap.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
    //cap.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);

    // wait for a new frame from camera and store it into 'frame'
    CaptureVideo::getInstance().cap().read(src);
    // check if we succeeded
    if (src.empty()) {
        std::cerr << "ERROR! blank frame grabbed\n";
        return -1;
    }

    this->cam_height = src.rows;
    this->cam_width = src.cols;

    cv::destroyWindow("Calibration");
    bool settingsOk = false;

    while (!settingsOk)
    {

        if (corners.size() == 4)
        {
            settingsOk = true;
        }
        cv::imshow("Screen pic", src);
        cv::waitKey(50);
        cv::setMouseCallback("Screen pic", CallBackMouseCorner, this);
        if (corners.size() > 0)
        {
            cv::circle(src, corners[corners.size()-1], 7, cv::Scalar(255, 255, 255), 3);
        }
    }
    cv::destroyWindow("Screen pic");


    if (corners.size() < 4)
    {
        return -1;
    }
    //On cherche la position des coins
    //en premier le top left -> on r�cup�re les deux plus hauts et celui le plus � gauche est s�lectionn�
    cv::Point2i pt1 = corners[0];
    int pose1 = 0;
    cv::Point2i pt2 = corners[1];
    int pose2 = 1;
    for (int i = 2; i < (int)corners.size(); i++)
    {
        cv::Point2i pt = corners[i];

        if (pt.y < pt1.y)
        {
            if (pt1.y < pt2.y)
            {
                pt2 = pt1;
                pose2 = pose1;
            }

            pt1 = pt;
            pose1 = i;

        }
        else if (pt.y < pt2.y)
        {
            pt2 = pt;
            pose2 = i;
        }
    }

    if (pt1.x < pt2.x)
    {
        topLeft[0] = pt1.x;
        topLeft[1] = this->cam_height - pt1.y;
        cv::circle(src, pt1, 7, cv::Scalar(255, 0, 0),3);
        cv::imshow("Corner", src);
        cv::waitKey(50);
        topRight[0] = pt2.x;
        topRight[1] = this->cam_height - pt2.y;
        cv::circle(src, pt2, 7, cv::Scalar(0, 0, 255),3);
        cv::imshow("Corner", src);
        cv::waitKey(50);
    }
    else
    {
        topLeft[0] = pt2.x;
        topLeft[1] = this->cam_height - pt2.y;
        cv::circle(src, pt2, 7, cv::Scalar(255, 0, 0),3);
        cv::imshow("Corner", src);
        cv::waitKey(50);
        topRight[0] = pt1.x;
        topRight[1] = this->cam_height - pt1.y;
        cv::circle(src, pt1, 7, cv::Scalar(0, 0, 255),3);
        cv::imshow("Corner", src);
        cv::waitKey(50);
    }

    //remove corners from vector with indexes
    if (pose1 < pose2)
    {
        corners.erase(corners.begin() + pose2);
        corners.erase(corners.begin() + pose1);
    }
    else
    {
        corners.erase(corners.begin() + pose1);
        corners.erase(corners.begin() + pose2);
    }

    pt1 = corners[0];
    pt2 = corners[1];

    //on ajoute les deux corners du bas
    if (pt1.x < pt2.x)
    {
        botLeft[0] = pt1.x;
        botLeft[1] = this->cam_height - pt1.y;
        cv::circle(src, pt1, 7, cv::Scalar(255, 0, 255),3);
        cv::imshow("Corner", src);
        cv::waitKey(50);
        botRight[0] = pt2.x;
        botRight[1] = this->cam_height - pt2.y;
        cv::circle(src, pt2, 7, cv::Scalar(0, 255, 0),3);
        cv::imshow("Corner", src);
        cv::waitKey(50);
    }
    else
    {
        botLeft[0] = pt2.x;
        botLeft[1] = this->cam_height - pt2.y;
        cv::circle(src, pt2, 7, cv::Scalar(255, 0,255),3);
        cv::imshow("Corner", src);
        cv::waitKey(50);
        botRight[0] = pt1.x;
        botRight[1] = this->cam_height - pt1.y;
        cv::circle(src, pt1, 7, cv::Scalar(0, 255, 0),3);
        cv::imshow("Corner", src);

    }
    cv::waitKey(1000);
    cv::destroyWindow("Corner");

    calibrated = true;

    point.push_back(cv::Point(botLeft[0],this->cam_height - botLeft[1]));
    point.push_back(cv::Point(topLeft[0],this->cam_height - topLeft[1]));
    point.push_back(cv::Point(topRight[0],this->cam_height - topRight[1]));
    point.push_back(cv::Point(botRight[0],this->cam_height - botRight[1]));

    /*https://www.particleincell.com/2012/quad-interpolation */
    //dans l'image camera -> coordonnees P(x,y)
    //dans le carre unite -> coordonnees P'(l,m)

    // x = a(0) + a(1)*l + a(2)*m + a(3)*l*m
    // y = b(0) + b(1)*l + b(2)*m + b(3)*l*m


    /*

    |x0|     |1 0 0 0|   |a0|
    |x1|  =  |1 1 0 0| * |a1|
    |x2|     |1 1 1 1|   |a2|
    |x3|     |1 0 1 0|   |a3|

    donc

    |a1|     | 1  0  0  0|   |x0|
    |a2|  =  |-1  1  0  0| * |x1|
    |a3|     |-1  0  0  1|   |x2|
    |a4|     | 1 -1  1 -1|   |x3|

    -> solve

    a1 = x0
    a2 = x1 - x0
    a3 = x3 - x0
    a4 = x0 - x1 + x2 - x3

    */

    a[0] = botLeft[0];
    a[1] = botRight[0] - botLeft[0];
    a[2] = topLeft[0] - botLeft[0];
    a[3] = botLeft[0] - botRight[0] + topRight[0] - topLeft[0];

    b[0] = botLeft[1];
    b[1] = botRight[1] - botLeft[1];
    b[2] = topLeft[1] - botLeft[1];
    b[3] = botLeft[1] - botRight[1] + topRight[1] - topLeft[1];

    return 0;
}

cv::Vec2f Camera::computeScreenToGame(cv::Vec2f point)
{

    /*https://www.particleincell.com/2012/quad-interpolation */
    //dans l'image camera -> coordonnees P(x,y)
    //dans le carre unite -> coordonnees P'(u,v)

    // x = a(0) + a(1)*u + a(2)*v + a(3)*u*v
    // y = b(0) + b(1)*u + b(2)*v + b(3)*u*v

    double x = point[0];
    double y = this->cam_height - point[1];

    double aa = a[3] * b[2] - a[2] * b[3];
    double bb = a[3] * b[0] - a[0] * b[3] + a[1] * b[2] - a[2] * b[1] + x * b[3] - y * a[3];
    double cc = a[1] * b[0] - a[0] * b[1] + x * b[1] - y * a[1];

    double det = std::sqrt(bb*bb-4*aa*cc);
    double u = (-bb + det) / (2 * aa);
    double v = (x - a[0] - (a[2] * u)) / (a[1] + (a[3] * u));


    cv::Vec2f ret;
    ret[1] = (1-u)*(float)height;
    ret[0] = v*(float)width;

    return ret;

}

int Camera::getWidth()
{
    return this->width;
}

int Camera::getHeight()
{
    return this->height;
}

int Camera::getCamWidth()
{
    return this->cam_width;
}
int Camera::getCamHeight()
{
    return this->cam_height;
}

std::vector<cv::Point> Camera::getPoints()
{
    return this->point;
}

void Camera::setWidth(int width_)
{
    this->width = width_;
}

void Camera::setHeight(int height_)
{
    this->height = height_;
}

bool Camera::getCalibrated()
{
    return calibrated;
}

TestCamPerso::TestCamPerso()
{
    cap.open(0, cv::CAP_ANY);
    assert(cap.isOpened());
}

bool TestCamPerso::getFrame(cv::Mat &rfFrame)
{
    return cap.read(rfFrame);
}

CaptureVideo* CaptureVideo::instance_ = nullptr;

CaptureVideo &CaptureVideo::getInstance()
{
    if (!instance_)
    {
        instance_ = new CaptureVideo();
    }

    return *instance_;
}

void CaptureVideo::delInstance()
{
    if (instance_)
        delete instance_;
}

const cv::VideoCapture &CaptureVideo::cap() const
{
    return *cap_;
}

cv::VideoCapture &CaptureVideo::cap()
{
    return *cap_;
}

CaptureVideo::CaptureVideo()
    : cap_(nullptr)
{
    cap_ = new cv::VideoCapture();
    cap_->open(0, cv::CAP_ANY);
    //assert(cap_->isOpened());
    cap_->set(cv::CAP_PROP_FRAME_WIDTH, 1920);
    cap_->set(cv::CAP_PROP_FRAME_HEIGHT, 1080);

}

CaptureVideo::~CaptureVideo()
{
    delete cap_;
}
