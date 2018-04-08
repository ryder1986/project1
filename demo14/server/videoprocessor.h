#ifndef VIDEOPROCESSOR_H
#define VIDEOPROCESSOR_H

#include <QObject>
#include <QJsonObject>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include "tool.h"
#include <QJsonValue>
#include <QJsonArray>
using namespace std;
using namespace cv;
class VideoProcessor
{

public:
   // explicit VideoProcessor(QObject *parent = 0);
    VideoProcessor(QJsonValue v)
    {
        area_2_rect(v);
    }



    virtual  bool real_process( Mat &, std::vector<cv::Rect> &)
    {

    }

    virtual void init()
    {

    }

    void area_2_rect(QJsonValue area)
    {
        int x_min=10000;
        int y_min=10000;
        int x_max=0;
        int y_max=0;
        foreach (QJsonValue v, area.toArray()) {
            int x= v.toObject()["x"].toInt();
            int y= v.toObject()["y"].toInt();
            if(x<x_min)
                x_min=x;
            if(x>x_max)
                x_max=x;
            if(y<y_min)
                y_min=y;
            if(y>y_max)
                y_max=y;
        }
        detect_area=Rect(x_min,y_min,x_max-x_min,y_max-y_min);
    }
    Rect rect()
    {
        return detect_area;
    }

protected:
    Rect detect_area;
};


#endif // VIDEOPROCESSOR_H
