#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cv.h>
#include <iostream>

#define VIDEO_WIDTH 1280 
#define VIDEO_HEIGHT 720
#define LIGHTNUM 8
#define RVALUETHRESHOLD 0

using namespace cv;
using namespace std;

void cvText(IplImage* src, string text, CvPoint p, CvScalar textColor);
int calRValue(CvPoint p, const IplImage *src);

class IMAGE {
private:
	IplImage *src;
	string fileName;
	long takeTime;

public:
	IMAGE() : src(NULL), fileName(""), takeTime(0) {}
	void fileConvert();
	void findLED(vector<CvPoint> locateionPoints);
	IplImage* getSrc() { return src; }
	long getTakeTime() { return takeTime; }
	void setFileName(string name) { fileName = name; }
	void clearState();
};
