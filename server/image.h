#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cv.h>

#define VIDEO_WIDTH 1280 
#define VIDEO_HEIGHT 720

using namespace cv;

class IMAGE {
private:

public:
	void fileConvert(string fileName);
	IplImage *src;

};