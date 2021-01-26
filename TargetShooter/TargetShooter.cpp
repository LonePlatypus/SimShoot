#include <iostream>
#include <time.h>

#include "Hit.h"
#include "game1.h"


using namespace std;
using namespace cv;



int main()
{

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


    //sortie du programme si click sur exit
    while (settings.getExit() == false)
    {
        switch (settings.getInput())
        {
        case 1 :
            hit.inputCamera();
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
        settings.displaySettings(&camera);


        clock_t time = clock() - t;
        clock_t frame_time = clock() - t_frame;
        double elapsed = ((double)time) / CLOCKS_PER_SEC;


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
            //�cran blanc ...
            else
            {
                display.display(1);
            }

        }

    }


    return 0;
}


