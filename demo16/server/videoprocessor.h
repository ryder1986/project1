#ifndef VIDEOPROCESSOR_H
#define VIDEOPROCESSOR_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include "tool.h"
using namespace std;
using namespace cv;
class VideoProcessor
{

public:
    VideoProcessor()
    {

    }
    virtual  bool real_process( Mat &, std::vector<cv::Rect> &)
    {
        return false;
    }

    virtual void init()
    {

    }
protected:
};


#endif // VIDEOPROCESSOR_H
