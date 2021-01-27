#include <iostream>
#include <time.h>

#include "Hit.h"
#include "game1.h"


using namespace std;
using namespace cv;



int main()
{
    std::cout << "OpenCV Version : " <<
                 CV_MAJOR_VERSION << "." << CV_MINOR_VERSION
              << "." << CV_SUBMINOR_VERSION << std::endl;

    TestCamPerso camPerso;
    cv::Mat frame;


    if (!camPerso.cap.isOpened())
    {
        std::cerr << "ERROR! Unable to open camera\n";
    }
    else
    {
        while(1)
        {
            clock_t timeA;
            clock_t timeB;
            double elapsedA=0.0;

            timeA= clock();
            camPerso.getFrame(frame);
//            cap.read(frame);
            //std::cout << camPerso.cap.getBackendName() << std::endl;

            timeB = clock();
            elapsedA = (double)(timeB-timeA) / CLOCKS_PER_SEC;
            std::cout <<"elapsed get image : "<<elapsedA<<std::endl;
            cv::imshow("debug",frame);
            cv::waitKey(5);

        }
    }


    //exit(127);


    //On met tout en place pour l'affichage
    Display display;

    Camera camera(0, display.getDisplayWidth() , display.getDisplayHeight());
    //camera.getTransformCamScreenSimple(display);
    //camera.DisplayCamera();

    //Les settings
    settings settings;
    //Les inputs (souris / cam�ra)
    Hit hit;
    hit.startVideoCap(0);
    //On lance le jeu 1
    game1 game(&settings, display.getTargetId(&settings));
    bool up = false;

    clock_t t;
    t = clock();
    clock_t t_frame;
    t_frame = t;


    clock_t timeA;
    clock_t timeB;
    double elapsedA=0.0;

    //sortie du programme si click sur exit
    while (settings.getExit() == false)
    {
        switch (settings.getInput())
        {
        case 1 :
            timeA= clock();
            hit.inputCamera();
            timeB = clock();
            elapsedA = (double)(timeB-timeA) / CLOCKS_PER_SEC;
            std::cout <<"elapsed : "<<elapsedA<<std::endl;

            for(int i = 0 ; i < hit.getHits().size() ; ++i)
            {

            }
            break;
        case 2 :
            hit.inputMouse();
            break;
        default:
            hit.inputMouse();
            break;
        }


        display.updateHit(&hit);
        settings.displaySettings(&camera , hit.getCamOpen());

        clock_t frame_time = clock() - t_frame;

        //rafraichissement � 30fps
        if (((double)frame_time) / CLOCKS_PER_SEC > 0.03)
        {
            t_frame = clock();

            //si on veut arr�ter le jeu
            if (settings.getStop() || game.getNbCible() < 0)
            {
                settings.setStop(false);
                game.stop(&display);
                //game.scores()
                game.reset();

            }
            //d�marage du jeu avec les settings courants
            else if (settings.getStart())
            {
                settings.setStart(false);
                game.start(&settings, display.getTargetId(&settings));
                game.countDownScreen(&display, 3);

            }
            //si le jeu est lanc�, on traite les tirs + l'affichage
            else if (game.getState() == 1)
            {
                game.update(&display);
            }
            else
            {
                display.display(1);
            }

        }

    }


    return 0;
}


