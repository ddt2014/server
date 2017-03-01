#pragma once 

#include <iostream>
#include <string>
#include "server.h"
#include "image.h"
//#include <WinBase.h>

#define PORT 44960
#define IP_ADDRESS "192.168.1.100"
#define CAMERANUM 2
#define SYNTIMES 5
#define TOTALLIGHTNUM 10

using namespace std;

void cvShowManyImages(SERVER server[], const vector<vector<int> > diffRes) {
	IplImage *DispImage;
	int scale = 2, temp;
	string text;

	// Create a new 3 channel image0  
	DispImage = cvCreateImage(cvSize(100 + VIDEO_WIDTH / scale * 2, 60 + VIDEO_HEIGHT / scale * 2), 8, 3);

	// Loop for CAMERANUM number of arguments  
	for (int i = 0, m = 20, n = 20; i < CAMERANUM; i++){
		// Used to Align the images  
		m += (i % 2) * (VIDEO_WIDTH / scale + 30);
		n += (i / 2) * (VIDEO_HEIGHT / scale + 30);
		cvSetImageROI(DispImage, cvRect(m, n, (int)(VIDEO_WIDTH / scale), (int)(VIDEO_HEIGHT / scale)));
		cvResize(server[i].getImage()->getSrc(), DispImage);
		cvResetImageROI(DispImage);
	}
	cvResetImageROI(DispImage);

	for (int i = 0; i < CAMERANUM - 1; ++i) {
		text = "Time difference betweent camera 1 and camera " + to_string(i + 2) + " : ";
		cvText(DispImage, text, cvPoint(300, 100 * i + VIDEO_HEIGHT / scale), cvScalar(0, 0, 255));
		for (int j = 0; j < SYNTIMES; ++j) {
			text = to_string(diffRes[i][j]);
			cvText(DispImage, text, cvPoint(810, 100 * i + 20 * j + VIDEO_HEIGHT / scale), cvScalar(0, 0, 255));
			temp = diffRes[i][j] % TOTALLIGHTNUM;
			if (temp < 0)
				temp += TOTALLIGHTNUM;
			text = to_string(temp);
			cvText(DispImage, text, cvPoint(880, 100 * i + 20 * j + VIDEO_HEIGHT / scale), cvScalar(0, 0, 255));
		}
	}

	for (int i = 0; i < CAMERANUM; ++i) {
		server[i].clearFileState();
	}

	cvShowImage("image", DispImage);
	cvWaitKey(0);
	cvReleaseImage(&DispImage);
	cvDestroyWindow("image");
}

void calDiffRes(SERVER server[], vector<vector<CvPoint> > locateionPoints, vector<vector<int> >& diffRes, int count) {
	for (int i = 0; i < CAMERANUM; ++i) {
		server[i].readSaveImage();
		server[i].getImage()->findLED(locateionPoints[i]);
		if (i > 0) {
			diffRes[i - 1][count] = (server[0].getImage()->getTakeTime() - server[i].getImage()->getTakeTime());
		}
	}
	if (count == SYNTIMES - 1)
		return;
	for (int i = 0; i < CAMERANUM; ++i) {
		server[i].clearFileState();
	}
}

int main(void) {
	SERVER server[CAMERANUM];
	bool flag;
	string command;
	cvNamedWindow("image");
	vector<vector<CvPoint> > locateionPoints(CAMERANUM, vector<CvPoint>(3));
	vector<vector<int> > diffRes(CAMERANUM - 1, vector<int>(SYNTIMES));

	locateionPoints[0][0] = cvPoint(597, 70);
	locateionPoints[0][1] = cvPoint(410, 67);
	locateionPoints[0][2] = cvPoint(408, 253);
	locateionPoints[1][0] = cvPoint(673, 168);
	locateionPoints[1][1] = cvPoint(491, 169);
	locateionPoints[1][2] = cvPoint(490, 353);

	for (int i = 0; i < CAMERANUM; ++i) {
		flag = server[i].initServer(PORT + i * 2, IP_ADDRESS, i);
		if (!flag) {
			cout << "creat server failed" << endl;
			return -1;
		}
	}
	flag = false;
	while (!flag) {
		flag = true;
		for (int i = 0; i < CAMERANUM; ++i)
			flag = flag & server[i].getState();
	}
	cout << "save picture: p \t save video: v-123" << endl;
	cout << "synchronize: y \t transfer file: f" << endl;
	cout << "change shutter speed us: h-20000 \t change FPS c-fps-frame: c-25-100" << endl;
	cout << "start camera: s \t stop camera: t" << endl;
	while (true) {
		cin >> command;
		if (command == "y") {
			for (int t = 0; t < SYNTIMES; ++t) {
				for (int i = 0; i < CAMERANUM; ++i) {
					server[i].sendMessage(command.c_str());
				}
				do {
					flag = true;
					for (int i = 0; i < CAMERANUM; ++i) {
						flag = flag & server[i].getFileState();
					}
				} while (!flag);
				for (int i = 0; i < CAMERANUM; ++i) {
					server[i].clearTransState();
				}
				calDiffRes(server, locateionPoints, diffRes, t);
			}
			cvShowManyImages(server, diffRes);
			continue;
		}
		else {
			for (int i = 0; i < CAMERANUM; ++i) {
				server[i].sendMessage(command.c_str());
			}
		}
	}
	return 0;
}