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


    //On met tout en place pour l'affichage
    Display display;

    Camera camera(0, display.getDisplayWidth() , display.getDisplayHeight());

    //Les settings
    settings settings;
    //Les inputs (souris / camera)
    Hit hit;
    hit.startVideoCap();

    //On lance l'enregistrement des résultats
    Record record;
    record.addPath("results.csv");

    //On lance le jeu 1
    game1 game(&settings, display.getTargetId(&settings));

    clock_t t;
    t = clock();
    clock_t t_frame;
    t_frame = clock();

    //sortie du programme si click sur exit
    while (settings.getExit() == false)
    {
        switch (settings.getInput())
        {
        case 1 :
            hit.inputCamera(&camera , settings.getGamma(), &record);
            break;
        case 2 :
            hit.inputMouse();
            break;
        default:
            hit.inputMouse();
            break;
        }


        display.updateHit(&hit , &record);
        settings.displaySettings(&camera , hit.getCamOpen());

        clock_t frame_time = clock() - t_frame;

        //rafraichissement a 30fps
        if (((double)frame_time) / CLOCKS_PER_SEC > 0.03)
        {
            t_frame = clock();

            //si on veut arreter le jeu
            if (settings.getStop() || game.getNbCible() < 0)
            {
                settings.setStop(false);
                game.stop(&display);
                //game.scores()
                game.reset();

            }
            //demarage du jeu avec les settings courants
            else if (settings.getStart())
            {
                settings.setStart(false);
                game.start(&settings, display.getTargetId(&settings));
                game.countDownScreen(&display, 3);
                record.setRecording(settings.getRecord());
                record.addSubFolder();

            }
            //si le jeu est lance, on traite les tirs + l'affichage
            else if (game.getState() == 1)
            {
                game.update(&display, &record);
            }
            else
            {
                display.display(1);
            }
        }

    }

    record.closeFile();
    CaptureVideo::delInstance();
    return 0;
}


