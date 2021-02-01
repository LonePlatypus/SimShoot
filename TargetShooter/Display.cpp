#include "Display.h"

#ifdef _WIN32
#  include <Windows.h>
#elif __linux__
//#  include <X11/Xlib.h>
#endif

#ifdef _WIN32
void getScreenSizeWin32(int& rfWidth,
                        int& rfHeight)
{
	rfWidth = (int)GetSystemMetrics(SM_CXSCREEN);
	rfHeight = (int)GetSystemMetrics(SM_CYSCREEN);
}
#endif

#ifdef __linux__
# include "Linux/screen.h"
void getScreenSizeLinux(int& rfWidth,
                        int& rfHeight)
{
  Linux_X11GetScreenSize(&rfWidth, &rfHeight);
}
#endif


void Display::GetScreenResoluton(
    int& rfWidth,
    int& rfHeight)
{
#ifdef _WIN32
  getScreenSizeWin32(rfWidth, rfHeight);
#elif __linux__
  getScreenSizeLinux(rfWidth, rfHeight);
#endif
}

Display::Display()
{
  GetScreenResoluton(display_width, display_height);

	background = cv::Mat::zeros(cv::Size(display_width, display_height), CV_8UC3);
	background = cv::Scalar(255, 255, 255);
    resetDisplayedbackground();

    cv::namedWindow(SHOOTING_NAME, cv::WINDOW_FULLSCREEN);

    loadTargets();
    loadHit();
}


//On rajoute une sprite sur le background avec les coordonnées
int Display::addTarget(int X, int Y, int targetId)
{
    cv::Mat overlay;
    if (targetId <0 || targetId > targets.size())
    {
        return -1;
    }
    overlay = targets[targetId];

    int overlay_height = overlay.rows;
    int overlay_width = overlay.cols;

    if (X - (int)(1 + overlay_width / 2) > 0 && Y - (int)(1 + overlay_height / 2) > 0 && X + (int)(1 + overlay_width / 2) < display_width && Y + (int)(1 + overlay_height / 2) < display_height)
    {
        cv::Rect center(X - overlay_width / 2, Y - overlay_height / 2, overlay_width, overlay_height);
        overlay.copyTo(displayed_background(center));
        return 0;
    }
    else
    {
        return -1;
    }
}

//On rajoute l'image d'impact (plus pour du débug qu'autre chose)
int Display::addHit(int X, int Y)
{
    if(X < 0 || Y <  0 || X > this->display_width || Y > this->display_height)
    {
        std::cerr << "Hit coords are oustide bounds : ["<<X<<";"<<Y<<"]"<<std::endl;
        return -1;
    }

    cv::Mat overlay;
    overlay = hit;

    int overlay_height = overlay.rows;
    int overlay_width = overlay.cols;

    if (X - (int)(1 + overlay_width / 2) > 0 && Y - (int)(1 + overlay_height / 2) > 0 && X + (int)(1 + overlay_width / 2) < display_width && Y + (int)(1 + overlay_height / 2) < display_height)
    {
        cv::Rect center(X - overlay_width / 2, Y - overlay_height / 2, overlay_width, overlay_height);

        //on gère la transparence
        cv::Mat roi = displayed_background(center);
        double alpha = 1.0;
        for (int r = 0; r < roi.rows; ++r)
        {
            for (int c = 0; c < roi.cols; ++c)
            {
                const cv::Vec4b& vf = overlay.at<cv::Vec4b>(r, c);
                if (vf[3] > 0) // alpha channel > 0
                {
                    // Blending
                    cv::Vec3b& vb = roi.at<cv::Vec3b>(r, c);
                    vb[0] = alpha * vf[0] + (1.0 - alpha) * vb[0];
                    vb[1] = alpha * vf[1] + (1.0 - alpha) * vb[1];
                    vb[2] = alpha * vf[2] + (1.0 - alpha) * vb[2];
                }
            }
        }


        //overlay.copyTo(displayed_background(center));

        return 0;
    }
    else
    {
        std::cout << "[Error] picture not added   X = "<<X<<"  | Y = "<<Y << std::endl;
        return -1;
    }

}


//On clean le fond d'image
int Display::resetDisplayedbackground()
{
    displayed_background = background;
    //parceque la ligne dessus ne marche pas ...
    displayed_background = cv::Scalar(255, 255, 255);

    return 0;
}

//On charge les deux images de cibles
int Display::loadTargets()
{
    //Jozy first for easter egg
    loadJPGTarget("./img/target_jozy.jpg", 1.);

    loadJPGTarget("./img/target_IPSC.jpg", (1.0/5.0));

    loadPNGTarget("./img/target_round.png", (1.0 /30.0));

    return 0;
}


//méthode dans le cas où c'est un PNG 4 canaux avec l'alpha
int Display::loadPNGTarget(std::string path, double ratio)
{
    cv::Mat img = imread(cv::samples::findFile(path), cv::IMREAD_UNCHANGED);
    resize(img, img, cv::Size(), ratio, ratio);

    //split des channels
    cv::Mat channels[4];
    split(img, channels);
    cv::Mat overlay = channels[3];

    //invert colors
    bitwise_not(overlay, img);
    cvtColor(img, img, cv::COLOR_GRAY2BGR);

    targets.push_back(img);
    return 0;
}

//cas JPG tranquille
int Display::loadJPGTarget(std::string path , double ratio)
{
    cv::Mat img = cv::imread(cv::samples::findFile(path), cv::IMREAD_COLOR);
    resize(img, img, cv::Size(), ratio, ratio);
    targets.push_back(img);
    return 0;
}


//On affiche l'image générale avec un délais
int Display::display(int delay)
{
    cv::imshow(SHOOTING_NAME, displayed_background);
    cv::waitKey(delay);
    return 0;
}


//import de l'image d'impact
int Display::loadHit()
{
    double ratio = 0.02;
    cv::Mat img = cv::imread("./img/hit.png", cv::IMREAD_UNCHANGED);
    resize(img, img, cv::Size(), ratio, ratio);

    hit = img;
    return 0;
}

int Display::getTargetId(settings *setting)
{
    if (setting->getTargetRound())
    {
        return 2;
    }
    else if (setting->getTargetISPC())
    {
        return 1;
    }else
    {
        return 0;
    }
}

//récupération de l'info de l'impact
int Display::updateHit(Hit* hit)
{

    if (hit && hit->getDetected())
    {
        hit->setDetected ( false);
        addHit(hit->getHitX(), hit->getHitY());
        return 0;
    }
    else
    {
        return 1;
    }
}

int Display::getDisplayWidth()
{
    return this->display_width;
}

int Display::getDisplayHeight()
{
    return this->display_height;
}

cv::Mat Display::getDisplayedBack()
{
    return this->displayed_background;
}
