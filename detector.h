#ifndef __DETECTOR__
#define __DETECTOR__

#include <iostream>
#include <string>
#include <vector>

#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>




#include <unistd.h>
#include<opencv2/opencv.hpp>  
#include<highgui.h>  
#include "opencv2/core/core.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;



typedef struct detector_param
{
    int my_flag;
    char my_label[256];
    Mat defects;
}my_detector;

int defects_detector(Mat src,my_detector* p_detector);
#endif