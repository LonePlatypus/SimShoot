#include "Camera.h"
#include <cassert>

cv::Vec2f lineIntersection (cv::Point2f A_start , cv::Point2f A_stop , cv::Point2f B_start , cv::Point2f B_stop)
{
    double x12 = A_start.x - A_stop.x;
    double x34 = B_start.x - B_stop.x;
    double y12 = A_start.y - A_stop.y;
    double y34 = B_start.y - B_stop.y;

    double det = x12*y34 - y12*x34;

    cv::Vec2f resultat;
    resultat[0] = -1.0;
    resultat[1] = -1.0;

    if(det != 0)
    {
        double a = A_start.x*A_stop.y - A_start.y*A_stop.x;
        double b = B_start.x*B_stop.y - B_start.y*B_stop.x;

        resultat[0] = (a*x34-b*x12) /det;
        resultat[1] = (a*y34+b*y12) /det;
    }

    std::cout <<"corner is : " << resultat[0] << "  ||  "<<resultat[1]<<std::endl;

    return resultat;
}

std::vector<cv::Point> linearRegression(std::vector<cv::Point> points)
{

    int datasetSize = points.size();

    double sum_x = 0.0;
    double sum_y = 0.0;
    double sum_xy = 0.0;
    double sum_xx = 0.0;

    for (int i = 0 ; i < datasetSize; ++i)
    {
        sum_x = sum_x + points[i].x;
        sum_y = sum_y + points[i].y;
        sum_xx = sum_xx + points[i].x*points[i].x;
        sum_xy = sum_xy + points[i].x*points[i].y;
    }

    //line equation is Y = a+ bX
    double b = ((datasetSize*sum_xy)-(sum_x*sum_y))/((datasetSize*sum_xx)-(sum_x*sum_x));
    double a = (sum_y-(b*sum_x))/datasetSize;

    std::vector<cv::Point> result;

    //we take X = -10000 && X = 10000
    cv::Point pt;
    pt.x = -10000;
    pt.y = a + b*pt.x;
    result.push_back(pt);

    pt.x = 10000;
    pt.y = a + b*pt.x;
    result.push_back(pt);

    return result;
}

void Camera::findCorners(    std::vector<cv::Point> leftBatch, std::vector<cv::Point> botBatch, std::vector<cv::Point> rightBatch, std::vector<cv::Point> topBatch)
{
    cv::Mat src;
    if (!CaptureVideo::getInstance().cap().isOpened()) {
        std::cerr << "ERROR! Unable to open camera\n";
    }

    // wait for a new frame from camera and store it into 'frame'
    CaptureVideo::getInstance().cap().read(src);
    CaptureVideo::getInstance().cap().read(src);
    // check if we succeeded
    if (src.empty()) {
        std::cerr << "ERROR! blank frame grabbed\n";;
    }

    std::vector<cv::Point2f> result;

    //do linear regression for each dataset
    std::vector<cv::Point> leftLine = linearRegression(leftBatch);
    std::vector<cv::Point> rightLine = linearRegression(rightBatch);
    std::vector<cv::Point> topLine = linearRegression(topBatch);
    std::vector<cv::Point> botLine = linearRegression(botBatch);


    //DEBUG
    cv::line(src,leftLine[0] , leftLine[1],cv::Scalar(0, 0, 255),3);
    cv::line(src,rightLine[0] , rightLine[1],cv::Scalar(0, 0, 255),3);
    cv::line(src,topLine[0] , topLine[1],cv::Scalar(0, 0, 255),3);
    cv::line(src,botLine[0] , botLine[1],cv::Scalar(0, 0, 255),3);

    //find corners with intersection
    topLeft = lineIntersection (leftLine[0] , leftLine[1] , topLine[0] , topLine[1]);
    topRight = lineIntersection (rightLine[0] , rightLine[1] , topLine[0] , topLine[1]);
    botLeft = lineIntersection (leftLine[0] , leftLine[1] , botLine[0] , botLine[1]);
    botRight = lineIntersection (rightLine[0] , rightLine[1] , botLine[0] , botLine[1]);


    cv::circle(src, cv::Point(topLeft[0], - topLeft[1]), 10, cv::Scalar(0, 255, 0),3);
    cv::circle(src, cv::Point(topRight[0],- topRight[1]), 10, cv::Scalar(255, 0, 0),3);
    cv::circle(src, cv::Point(botLeft[0],- botLeft[1]), 10, cv::Scalar(255, 255, 0),3);
    cv::circle(src, cv::Point(botRight[0],- botRight[1]), 10, cv::Scalar(0, 255, 255),3);

    cv::imshow("DEBUG",src);
    cv::waitKey(5000);


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


}

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

int Camera::getCameraIntrinsicparameters()
{

    //on fait la calibration caméra
    int CHECKERBOARD[2]{6,9};

    // Creating vector to store vectors of 3D points for each checkerboard image
    std::vector<std::vector<cv::Point3f> > objpoints;

    // Creating vector to store vectors of 2D points for each checkerboard image
    std::vector<std::vector<cv::Point2f> > imgpoints;

    // Defining the world coordinates for 3D points
    std::vector<cv::Point3f> objp;

    for(int i{0}; i<CHECKERBOARD[1]; i++)
    {
        for(int j{0}; j<CHECKERBOARD[0]; j++)
            objp.push_back(cv::Point3f(j,i,0));
    }


    //on affiche le chessboard
    cv::Mat chessboardBackground;
    chessboardBackground = cv::Mat::zeros(cv::Size(width, height), CV_8UC3);
    std::string path = "./img/Calibration/pattern.jpg";
    cv::Mat chessboard = imread(cv::samples::findFile(path), cv::IMREAD_COLOR);
    resize(chessboard, chessboard, cv::Size(), 0.7, 0.7);
    cv::Mat tmpchessboard = chessboard.clone();

    int img_nbr = 50;
    for (int i = 0 ;  i < img_nbr ; i++)
    {
        //update display
        tmpchessboard = chessboard.clone();
        putText(tmpchessboard, std::to_string(i+1) + "/" + std::to_string(img_nbr), cv::Point(800,600), cv::FONT_HERSHEY_SIMPLEX,1, cv::Scalar(0,0,255), 2);
        cv::imshow("Chessboard", tmpchessboard);
        cv::waitKey(600);
        cv::imshow("Chessboard", chessboard);
        cv::waitKey(600);
        cv::Mat img_chess;

        if (!CaptureVideo::getInstance().cap().isOpened())
        {
            std::cerr << "ERROR! Unable to open camera\n";
            return -1;
        }
        CaptureVideo::getInstance().cap().read(img_chess);
        CaptureVideo::getInstance().cap().read(img_chess);

        // check if we succeeded
        if (img_chess.empty())
        {
            std::cerr << "ERROR! blank frame grabbed\n";
            return -1;
        }

        cv::imwrite("./img/Calibration/img/"+std::to_string(i+1)+".jpg",img_chess);
        cv::imshow("Picture", img_chess);
        cv::waitKey(500);
        cv::destroyWindow("Picture");
    }

    cv::destroyWindow("Chessboard");

    // Extracting path of individual image stored in a given directory
    std::vector<cv::String> images;
    // Path of the folder containing checkerboard images
    std::string path_imgs = "./img/Calibration/img/*.jpg";

    cv::glob(path_imgs, images);

    cv::Mat frame, gray;
    // vector to store the pixel coordinates of detected checker board corners
    std::vector<cv::Point2f> corner_pts;
    bool success;

    // Looping over all the images in the directory
    for(int i{0}; i<(int)images.size(); i++)
    {
        frame = cv::imread(images[i]);
        cv::cvtColor(frame,gray,cv::COLOR_BGR2GRAY);

        // Finding checker board corners
        // If desired number of corners are found in the image then success = true
        success = cv::findChessboardCorners(gray, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, cv::CALIB_CB_ADAPTIVE_THRESH| cv::CALIB_CB_FAST_CHECK| cv::CALIB_CB_NORMALIZE_IMAGE);

        /*
       * If desired number of corner are detected,
       * we refine the pixel coordinates and display
       * them on the images of checker board
      */
        if(success)
        {
            cv::TermCriteria criteria(cv::TermCriteria::EPS | cv::TermCriteria::MAX_ITER, 30, 0.001);

            // refining pixel coordinates for given 2d points.
            cv::cornerSubPix(gray,corner_pts,cv::Size(11,11), cv::Size(-1,-1),criteria);

            // Displaying the detected corner points on the checker board
            cv::drawChessboardCorners(frame, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, success);

            objpoints.push_back(objp);
            imgpoints.push_back(corner_pts);
        }

        cv::imshow("Image",frame);
        cv::waitKey(100);
    }

    cv::destroyAllWindows();

    cv::Mat cameraMatrix,distCoeffs,R,T;

    /*
     * Performing camera calibration by
     * passing the value of known 3D points (objpoints)
     * and corresponding pixel coordinates of the
     * detected corners (imgpoints)
    */
    cv::calibrateCamera(objpoints, imgpoints, cv::Size(gray.rows,gray.cols), cameraMatrix, distCoeffs, R, T);

    cam_matrix = cameraMatrix;
    cam_dist = distCoeffs;

    std::cout << "cameraMatrix : " << cameraMatrix << std::endl;
    std::cout << "distCoeffs : " << distCoeffs << std::endl;
    //std::cout << "Rotation vector : " << R << std::endl;
    //std::cout << "Translation vector : " << T << std::endl;


    //DEBUG
    cv::Mat raw_img;
    cv::Mat str_img;

    CaptureVideo::getInstance().cap().read(raw_img);
    undistort(raw_img, str_img, cam_matrix, cam_dist);

    cv::imshow("raw_img", raw_img);
    cv::imshow("str_img", str_img);
    cv::waitKey(1000);
    cv::destroyAllWindows();
    //FIN DEBUG

    return 0;
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

    // wait for a new frame from camera and store it into 'frame'
    CaptureVideo::getInstance().cap().read(src);
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
        botLeft[0] = pt1.x;
        botLeft[1] = this->cam_height - pt1.y;
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

int Camera::getTransformCamScreenAutomatic()
{

    //On construit l'image blanche avec des cercles noirs aux coins
    cv::Mat img;
    img = cv::Mat::zeros(cv::Size(width, height), CV_8UC3);
    img = cv::Scalar(255, 255, 255);


    int radius = height/10;

    //double ratioFillEmpty = 6/10;

    int nbVert = 4;
    int nbHorz = 6;

    cv::Mat gray;
    cv::Mat src;
    std::vector<cv::Vec3f> circles;


    double HoughParamA = 210;
    double HoughParamB = 60;

    std::vector<cv::Point> leftBatch;
    std::vector<cv::Point> botBatch;
    std::vector<cv::Point> rightBatch;
    std::vector<cv::Point> topBatch;

    for(int k = 0; k < 4 ; ++k )
    {
        img = cv::Scalar(255, 255, 255);

        switch (k) {
        case 0:
            //create left line
            for(int i = 1 ; i < nbVert ; i++)
            {
                cv::circle( img,cv::Point(0,i*height/nbVert),radius,cv::Scalar( 0, 0, 0 ),cv::FILLED,cv::LINE_8);
            }
            cv::imshow("CamToScreenCalibration", img);
            cv::waitKey(1000);

            if (!CaptureVideo::getInstance().cap().isOpened()) {
                std::cerr << "ERROR! Unable to open camera\n";
                return -1;
            }

            // wait for a new frame from camera and store it into 'frame'
            CaptureVideo::getInstance().cap().read(src);
            CaptureVideo::getInstance().cap().read(src);
            // check if we succeeded
            if (src.empty()) {
                std::cerr << "ERROR! blank frame grabbed\n";
                return -1;
            }

            this->cam_height = src.rows;
            this->cam_width = src.cols;

            cv::destroyWindow("CamToScreenCalibration");
            cv::imshow("detected", src);
            cv::waitKey(1000);
            //detect circles
            cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
            cv::GaussianBlur( gray, gray, cv::Size(9, 9), 2, 2 );

            cv::HoughCircles(gray, circles, cv::HOUGH_GRADIENT, 1.5, radius, HoughParamA,HoughParamB,20, radius);


            if((int)circles.size() == nbVert-1)
            {
                for( size_t i = 0; i < circles.size(); i++ )
                {
                    cv::Vec3i c = circles[i];
                    cv::Point pt = cv::Point(c[0], c[1]);
                    leftBatch.push_back(pt);
                }
            }


            //DEBUG
            for( size_t i = 0; i < circles.size(); i++ )
            {
                cv::Vec3i c = circles[i];
                cv::Point center = cv::Point(c[0], c[1]);
                // circle center
                cv::circle( src, center, 1, cv::Scalar(0,100,100), 3, cv::LINE_AA);
                // circle outline
                int radius = c[2];
                cv::circle( src, center, radius, cv::Scalar(255,0,255), 3, cv::LINE_AA);
            }
            cv::imshow("detected", src);
            cv::waitKey(500);

            break;

        case 1:

            //create right line
            for(int i = 1 ; i < nbVert ; i++)
            {
                cv::circle( img,cv::Point(width,i*height/nbVert),radius,cv::Scalar( 0, 0, 0 ),cv::FILLED,cv::LINE_8);
            }
            cv::imshow("CamToScreenCalibration", img);
            cv::waitKey(1000);


            if (!CaptureVideo::getInstance().cap().isOpened()) {
                std::cerr << "ERROR! Unable to open camera\n";
                return -1;
            }

            // wait for a new frame from camera and store it into 'frame'
            CaptureVideo::getInstance().cap().read(src);
            CaptureVideo::getInstance().cap().read(src);
            // check if we succeeded
            if (src.empty()) {
                std::cerr << "ERROR! blank frame grabbed\n";
                return -1;
            }

            this->cam_height = src.rows;
            this->cam_width = src.cols;

            cv::destroyWindow("CamToScreenCalibration");
            cv::imshow("detected", src);
            cv::waitKey(1000);
            //detect circles

            cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
            cv::GaussianBlur( gray, gray, cv::Size(9, 9), 2, 2 );

            cv::HoughCircles(gray, circles, cv::HOUGH_GRADIENT, 1.5, radius, HoughParamA,HoughParamB,20, radius);

            if((int)circles.size() == nbVert-1)
            {
                for( size_t i = 0; i < circles.size(); i++ )
                {
                    cv::Vec3i c = circles[i];
                    cv::Point pt = cv::Point(c[0], c[1]);
                    rightBatch.push_back(pt);
                }
            }

            //DEBUG
            for( size_t i = 0; i < circles.size(); i++ )
            {
                cv::Vec3i c = circles[i];
                cv::Point center = cv::Point(c[0], c[1]);
                // circle center
                cv::circle( src, center, 1, cv::Scalar(0,100,100), 3, cv::LINE_AA);
                // circle outline
                int radius = c[2];
                cv::circle( src, center, radius, cv::Scalar(255,0,255), 3, cv::LINE_AA);
            }
            cv::imshow("detected", src);
            cv::waitKey(500);

            break;

        case 2:
            //create top  line
            for(int i = 1 ; i < nbHorz ; i++)
            {
                cv::circle( img,cv::Point(i*width/nbHorz,0),radius,cv::Scalar( 0, 0, 0 ),cv::FILLED,cv::LINE_8);
            }
            cv::imshow("CamToScreenCalibration", img);
            cv::waitKey(1000);

            if (!CaptureVideo::getInstance().cap().isOpened()) {
                std::cerr << "ERROR! Unable to open camera\n";
                return -1;
            }

            // wait for a new frame from camera and store it into 'frame'
            CaptureVideo::getInstance().cap().read(src);
            CaptureVideo::getInstance().cap().read(src);
            // check if we succeeded
            if (src.empty()) {
                std::cerr << "ERROR! blank frame grabbed\n";
                return -1;
            }

            this->cam_height = src.rows;
            this->cam_width = src.cols;

            cv::destroyWindow("CamToScreenCalibration");
            cv::imshow("detected", src);
            cv::waitKey(1000);

            //detect circles
            cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
            cv::GaussianBlur( gray, gray, cv::Size(9, 9), 2, 2 );

            cv::HoughCircles(gray, circles, cv::HOUGH_GRADIENT, 1.5, radius, HoughParamA,HoughParamB,20, radius);

            if((int)circles.size() == nbHorz-1)
            {
                for( size_t i = 0; i < circles.size(); i++ )
                {
                    cv::Vec3i c = circles[i];
                    cv::Point pt = cv::Point(c[0], c[1]);
                    topBatch.push_back(pt);
                }
            }

            //DEBUG
            for( size_t i = 0; i < circles.size(); i++ )
            {
                cv::Vec3i c = circles[i];
                cv::Point center = cv::Point(c[0], c[1]);
                // circle center
                cv::circle( src, center, 1, cv::Scalar(0,100,100), 3, cv::LINE_AA);
                // circle outline
                int radius = c[2];
                cv::circle( src, center, radius, cv::Scalar(255,0,255), 3, cv::LINE_AA);
            }
            cv::imshow("detected", src);
            cv::waitKey(500);

            break;

        case 3:
            //create bottom line
            for(int i = 1 ; i < nbHorz ; i++)
            {
                cv::circle( img,cv::Point(i*width/nbHorz,height),radius,cv::Scalar( 0, 0, 0 ),cv::FILLED,cv::LINE_8);
            }
            cv::imshow("CamToScreenCalibration", img);
            cv::waitKey(1000);

            if (!CaptureVideo::getInstance().cap().isOpened()) {
                std::cerr << "ERROR! Unable to open camera\n";
                return -1;
            }

            // wait for a new frame from camera and store it into 'frame'
            CaptureVideo::getInstance().cap().read(src);
            CaptureVideo::getInstance().cap().read(src);
            // check if we succeeded
            if (src.empty()) {
                std::cerr << "ERROR! blank frame grabbed\n";
                return -1;
            }

            this->cam_height = src.rows;
            this->cam_width = src.cols;

            cv::destroyWindow("CamToScreenCalibration");
            cv::imshow("detected", src);
            cv::waitKey(1000);

            //detect circles
            cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
            cv::GaussianBlur( gray, gray, cv::Size(9, 9), 2, 2 );

            cv::HoughCircles(gray, circles, cv::HOUGH_GRADIENT, 1.5, radius, HoughParamA,HoughParamB,20, radius);

            if((int)circles.size() == nbHorz-1)
            {
                for( size_t i = 0; i < circles.size(); i++ )
                {
                    cv::Vec3i c = circles[i];
                    cv::Point pt = cv::Point(c[0], c[1]);
                    botBatch.push_back(pt);
                }
            }

            //DEBUG
            for( size_t i = 0; i < circles.size(); i++ )
            {
                cv::Vec3i c = circles[i];
                cv::Point center = cv::Point(c[0], c[1]);
                // circle center
                cv::circle( src, center, 1, cv::Scalar(0,100,100), 3, cv::LINE_AA);
                // circle outline
                int radius = c[2];
                cv::circle( src, center, radius, cv::Scalar(255,0,255), 3, cv::LINE_AA);
            }
            cv::imshow("detected", src);
            cv::waitKey(500);
            cv::destroyWindow("detected");
            break;

        default:
            std::cout <<"[ERROR]"<<std::endl;
            break;

        }

    }

    if((int)topBatch.size()+(int)botBatch.size()+(int)leftBatch.size()+(int)rightBatch.size() == 2*(nbVert + nbHorz - 2))
    {

        //Do line intersections
        findCorners(leftBatch, botBatch, rightBatch, topBatch);


    }
    else
    {
        //maybe run the manual version ...
        getTransformCamScreenSimple();
    }

    return 0;

}



int Camera::getTransformCamScreenAutomaticArUco()
{
    cv::Mat img;
    img = cv::Mat::zeros(cv::Size(width, height), CV_8UC3);
    img = cv::Scalar(255, 255, 255);

    cv::Mat imgAR0 = cv::imread("./img/aruco_0.png");
    imgAR0 = imgAR0(cv::Range(50,700), cv::Range(50,700));
    resize(imgAR0, imgAR0, cv::Size(), 0.5, 0.5);
    cv::Mat imgAR1 = cv::imread("./img/aruco_1.png");
    imgAR1 = imgAR1(cv::Range(50,700), cv::Range(50,700));
    resize(imgAR1, imgAR1, cv::Size(), 0.5, 0.5);
    cv::Mat imgAR2 = cv::imread("./img/aruco_2.png");
    imgAR2 = imgAR2(cv::Range(50,700), cv::Range(50,700));
    resize(imgAR2, imgAR2, cv::Size(), 0.5, 0.5);
    cv::Mat imgAR3 = cv::imread("./img/aruco_3.png");
    imgAR3 = imgAR3(cv::Range(50,700), cv::Range(50,700));
    resize(imgAR3, imgAR3, cv::Size(), 0.5, 0.5);

    cv::Mat destROI = img(cv::Rect(0,0,imgAR0.cols,imgAR0.rows));
    imgAR0.copyTo(destROI);

    destROI = img(cv::Rect(img.cols-imgAR1.cols,0,imgAR1.cols,imgAR1.rows));
    imgAR1.copyTo(destROI);

    destROI = img(cv::Rect(0,img.rows-imgAR2.rows-25,imgAR2.cols,imgAR2.rows));
    imgAR2.copyTo(destROI);

    destROI = img(cv::Rect(img.cols-imgAR3.cols,img.rows-imgAR3.rows-25,imgAR3.cols,imgAR3.rows));
    imgAR3.copyTo(destROI);

    cv::imshow("Calibration", img);
    cv::waitKey(1000);

    cv::Mat src;
    if (!CaptureVideo::getInstance().cap().isOpened()) {
        std::cerr << "ERROR! Unable to open camera\n";
        return -1;
    }

    // wait for a new frame from camera and store it into 'frame'
    CaptureVideo::getInstance().cap().read(src);
    CaptureVideo::getInstance().cap().read(src);
    // check if we succeeded
    if (src.empty()) {
        std::cerr << "ERROR! blank frame grabbed\n";
        return -1;
    }

    this->cam_height = src.rows;
    this->cam_width = src.cols;

    cv::destroyWindow("Calibration");

    //detect aruco markers
    std::vector<aruconano::Marker> markers=aruconano::MarkerDetector::detect(src);
    for(const auto &m:markers)
    {
        m.draw(src);
    }

    if (markers.size() != 4)
    {
        //echec
        getTransformCamScreenSimple();
    }
    else
    {

        aruconano::Marker marker = markers[0];
        cv::Point2f corner = marker[0];
        topLeft[0] = corner.x;
        topLeft[1] = this->cam_height - corner.y;

        marker = markers[1];
        corner = marker[0];
        topRight[0] = corner.x;
        topRight[1] = this->cam_height - corner.y;

        marker = markers[2];
        corner = marker[0];
        botLeft[0] = corner.x;
        botLeft[1] = this->cam_height - corner.y;

        marker = markers[3];
        corner = marker[0];
        botRight[0] = corner.x;
        botRight[1] = this->cam_height - corner.y;

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
    }

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
