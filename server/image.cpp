#include "image.h"

void IMAGE :: fileConvert() {
	FILE *fp = fopen(fileName.c_str(), "rb");
	char *buffer = (char *)malloc(VIDEO_WIDTH * VIDEO_HEIGHT * 3 / 2);
	IplImage *yimg, *uimg, *vimg, *uuimg, *vvimg;

	src = cvCreateImage(cvSize(VIDEO_WIDTH, VIDEO_HEIGHT), IPL_DEPTH_8U, 3);
	yimg = cvCreateImageHeader(cvSize(VIDEO_WIDTH, VIDEO_HEIGHT), IPL_DEPTH_8U, 1);    // 亮度分量
	uimg = cvCreateImageHeader(cvSize(VIDEO_WIDTH / 2, VIDEO_HEIGHT / 2), IPL_DEPTH_8U, 1);  // 这两个都是色度分量
	vimg = cvCreateImageHeader(cvSize(VIDEO_WIDTH / 2, VIDEO_HEIGHT / 2), IPL_DEPTH_8U, 1);
	uuimg = cvCreateImage(cvSize(VIDEO_WIDTH, VIDEO_HEIGHT), IPL_DEPTH_8U, 1);
	vvimg = cvCreateImage(cvSize(VIDEO_WIDTH, VIDEO_HEIGHT), IPL_DEPTH_8U, 1);

	fread(buffer, sizeof(char), VIDEO_WIDTH * VIDEO_HEIGHT * 3 / 2, fp);
	cvSetData(yimg, buffer, VIDEO_WIDTH);
	cvSetData(uimg, buffer + VIDEO_WIDTH * VIDEO_HEIGHT, VIDEO_WIDTH / 2);
	cvSetData(vimg, buffer + VIDEO_WIDTH * VIDEO_HEIGHT * 5 / 4, VIDEO_WIDTH / 2);

	cvResize(uimg, uuimg, CV_INTER_LINEAR);
	cvResize(vimg, vvimg, CV_INTER_LINEAR);
	cvMerge(yimg, vvimg, uuimg, NULL, src);   // 合并单通道为三通道
	cvCvtColor(src, src, CV_YUV2BGR);

	fileName += ".jpg";
	cvSaveImage(fileName.c_str(), src);
	//cvShowImage("yuv", rgbimg);
	//cvWaitKey(0);

	fclose(fp);
	free(buffer);
	cvReleaseImage(&uuimg);
	cvReleaseImage(&vvimg);
	cvReleaseImageHeader(&yimg);
	cvReleaseImageHeader(&uimg);
	cvReleaseImageHeader(&vimg);
}

int calRValue(CvPoint p, const IplImage *src) { //统计LED中心周围正方形RGB值
	long res = 0;
	for (int i = p.x - RVALUETHRESHOLD; i <= p.x + RVALUETHRESHOLD; ++i) {
		for (int j = p.y - RVALUETHRESHOLD; j <= p.y + RVALUETHRESHOLD; ++j) {
			res += ((uchar *)(src->imageData + j*src->widthStep))[i*src->nChannels + 1];;
		}
	}
	return res / (2 * RVALUETHRESHOLD + 1) / (2 * RVALUETHRESHOLD + 1);
}

void cvText(IplImage* src, string text, CvPoint p, CvScalar textColor)
{
	CvFont font;
	double hscale = 0.6;
	double vscale = 0.6;
	int linewidth = 2;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX | CV_FONT_ITALIC, hscale, vscale, 0, linewidth);
	cvPutText(src, text.c_str(), p, &font, textColor);
}

void IMAGE :: findLED(vector<CvPoint> locateionPoints) {
	if (src == NULL)
		return;

	float x1, y1, x2, y2, x, y;
	long RTotal = 0, multi = 1;
	int start = 0, end = 0;
	takeTime = 0;

	vector<vector<CvPoint> > LEDMatrix(LIGHTNUM, vector<CvPoint>(LIGHTNUM));
	vector<vector<int> > LEDRValue(LIGHTNUM, vector<int>(LIGHTNUM));
	//vector<vector<bool> > LEDRes(LIGHTNUM, vector<bool>(LIGHTNUM));
	vector<int> LEDLine(LIGHTNUM);

	x1 = (float)(locateionPoints[0].x - locateionPoints[1].x) / (float)(LIGHTNUM + 1);//左上和右上定位点之间xy差
	x2 = (float)(locateionPoints[2].x - locateionPoints[1].x) / (float)(LIGHTNUM + 1);//左上和左下
	y1 = (float)(locateionPoints[0].y - locateionPoints[1].y) / (float)(LIGHTNUM + 1);
	y2 = (float)(locateionPoints[2].y - locateionPoints[1].y) / (float)(LIGHTNUM + 1);
	for (int i = 0; i < locateionPoints.size(); ++i) {
		cvCircle(src, locateionPoints[i], 5, CV_RGB(255, 0, 0), 2);
	}

	//计算LED中心坐标，统计G值
	for (int i = 0; i < LIGHTNUM; ++i) {
		x = locateionPoints[1].x + x2 * (i + 1);
		y = locateionPoints[1].y + y2 * (i + 1);
		for (int j = 0; j < LIGHTNUM; ++j) {
			LEDMatrix[i][j].x = (int)(x + x1 * (j + 1));
			LEDMatrix[i][j].y = (int)(y + y1 * (j + 1));
			LEDRValue[i][j] = calRValue(LEDMatrix[i][j], src);
			RTotal += LEDRValue[i][j];
			//cout << LEDRValue[i][j] << "\t";
			cvCircle(src, LEDMatrix[i][j], 3, CV_RGB(255, 0, 0), 1);
		}
		//cout << endl;
	}
	RTotal = RTotal / LIGHTNUM / LIGHTNUM;
	//cout << RTotal << endl;

	//画出检测结果
	for (int i = 0; i < LIGHTNUM; ++i) {
		start = 0;
		end = 0;
		for (int j = 0; j < LIGHTNUM; ++j) {

			if (LEDRValue[i][j] > RTotal) {
				cvCircle(src, cvPoint(LEDMatrix[i][j].x - 200, LEDMatrix[i][j].y), 7, CV_RGB(255, 0, 0), CV_FILLED);
				//LEDRes[i][j] = true;
				if ((start == 0 && end == 0) || (end != 0 && start != 0)) {
					start = j + 1;
					end = 0;
				}
			}
			else {
				cvCircle(src, cvPoint(LEDMatrix[i][j].x - 200, LEDMatrix[i][j].y), 7, CV_RGB(255, 0, 0), 2);
				//LEDRes[i][j] = false;
				if (start != 0) {
					end = j + 1;
				}
			}
		}
		if (start == 1 && end == 0)
			LEDLine[i] = 0;
		else
			LEDLine[i] = start;
		//cout << start << end << endl;
		cvText(src, to_string(LEDLine[i]), cvPoint(LEDMatrix[i][0].x - 250, LEDMatrix[i][0].y), cvScalar(0, 255, 255));
		if (i > 0 && i < 5) {
			takeTime += LEDLine[i] * multi; 
			multi = multi << 3;
		}
	}
	cvText(src, to_string(takeTime), cvPoint(LEDMatrix[0][0].x - 350, LEDMatrix[0][0].y), cvScalar(0, 255, 255));
	//cvShowImage("", src);
	cvSaveImage(fileName.c_str(), src);
}

void IMAGE::clearState() {
	fileName.clear();
	takeTime = 0;
	src = NULL;
}