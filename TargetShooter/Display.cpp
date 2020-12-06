#include "Display.h"

Display::Display()
{

	display_width = (int)GetSystemMetrics(SM_CXSCREEN);
	display_height = (int)GetSystemMetrics(SM_CYSCREEN);

	background = cv::Mat::zeros(cv::Size(display_width, display_height), CV_8UC3);
	background = cv::Scalar(255, 255, 255);
    resetDisplayedbackground();

    cv::namedWindow(SHOOTING_NAME, cv::WINDOW_FULLSCREEN);

    loadTargets();
    loadHit();

}


//On rajoute une sprite sur le background avec les coordonn�es
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

//On rajoute l'image d'impact (plus pour du d�bug qu'autre chose)
int Display::addHit(int X, int Y)
{

    std::cout << "in add Hit" << std::endl;
    cv::Mat overlay;
    overlay = hit;

    int overlay_height = overlay.rows;
    int overlay_width = overlay.cols;

    if (X - (int)(1 + overlay_width / 2) > 0 && Y - (int)(1 + overlay_height / 2) > 0 && X + (int)(1 + overlay_width / 2) < display_width && Y + (int)(1 + overlay_height / 2) < display_height)
    {
        cv::Rect center(X - overlay_width / 2, Y - overlay_height / 2, overlay_width, overlay_height);

        //on g�re la transparence
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
    loadJPGTarget("C:/Users/Sigma/Pictures/target_IPSC.jpg", (1.0/5.0));

    loadPNGTarget("C:/Users/Sigma/Pictures/target_round.png", (1.0 /30.0));

    return 0;
}


//m�thode dans le cas o� c'est un PNG 4 canaux avec l'alpha
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


//On affiche l'image g�n�rale avec un d�lais
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
    cv::Mat img = cv::imread("C:/Users/Sigma/Pictures/hit.png", cv::IMREAD_UNCHANGED);
    resize(img, img, cv::Size(), ratio, ratio);

    hit = img;
    return 0;
}

int Display::getTargetId(settings *setting)
{
    if (setting->Target_round)
    {
        return 1;
    }
    else if (setting->Target_ISPC)
    {
        return 0;
    }else
    {
        return 0;
    }
}

//r�cup�ration de l'info de l'impact
int Display::updateHit(Hit* hit)
{
    if (hit->detected)
    {
        hit->detected = false;
        addHit(hit->xHit, hit->yHit);
        return 0;
    }
    else
    {
        return 1;
    }
}