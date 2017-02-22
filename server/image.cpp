#include "image.h"

void IMAGE :: fileConvert(string fileName) {
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
	cvMerge(yimg, uuimg, vvimg, NULL, src);   // 合并单通道为三通道
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
	cvDestroyWindow("yuv");
}
