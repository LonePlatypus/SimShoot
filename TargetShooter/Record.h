#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/filesystem.hpp>

using namespace std::chrono;

class Record
{
private :

    bool recording;

    std::string path;
    std::ofstream file;

     unsigned __int64 subFolderSession;

public :

    Record();
    void addPath(std::string path);
    void addSubFolder();
    bool openFile();
    void addHit(double x, double y);
    void addTarget(double x, double y, int type);
    void setRecording(bool state);
    bool getRecording();
    void recordImg(cv::Mat &img);
    void closeFile();

};

