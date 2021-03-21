#include "Record.h"

Record::Record()
{
    recording = false;
    this->path = "";
    this->subFolderSession = 0;
}

void Record::addPath(std::string path)
{
    this->path = path;
}

void Record::addSubFolder()
{
    if(recording)
    {
        if(this->file.is_open())
        {
            file.close();
        }
        unsigned __int64 now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        subFolderSession = now;
        cv::utils::fs::createDirectory(std::to_string(subFolderSession));
        openFile();
    }
}

bool Record::openFile()
{
    if(path != "")
    {
        this->file.open(std::to_string(subFolderSession)+"/"+path);
        if(this->file.is_open())
        {
            file <<"Time(ms);Type;X;Y\n";
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

void Record::addHit(double x, double y)
{
    if(file.is_open())
    {
        unsigned __int64 now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        file <<now<<";"<<-1<<";"<<x<<";"<<y<< "\n";
    }
}

void Record::addTarget(double x, double y, int type)
{
    if(file.is_open())
    {
        unsigned __int64 now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        file <<now<<";"<<type<<";"<<x<<";"<<y<< "\n";
    }
}

void Record::recordImg(cv::Mat& img)
{
    std::string path = std::to_string(subFolderSession) + "/img";
    unsigned __int64 name = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - subFolderSession;
    path = path + std::to_string(name) +".png";
    cv::imwrite(path,img);
}

void Record::closeFile()
{
    file.close();
}

void Record::setRecording(bool state)
{
    recording = state;
}

bool Record::getRecording()
{
    return recording;
}
