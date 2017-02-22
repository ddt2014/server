#include <iostream>
#include <string>
#include "server.h"
#include "image.h"

#define PORT 44960
#define IP_ADDRESS "192.168.1.100"
#define CAMERANUM 2

using namespace std;

void cvShowManyImages(SERVER server[]) {
	IplImage *DispImage;
	int scale = 2.0;

	// Create a new 3 channel image0  
	DispImage = cvCreateImage(cvSize(100 + VIDEO_WIDTH / scale * 2, 60 + VIDEO_HEIGHT / scale * 2), 8, 3);

	// Loop for CAMERANUM number of arguments  
	for (int i = 0, m = 20, n = 20; i < CAMERANUM; i++){
		// Used to Align the images  
		m += (i % 2) * (VIDEO_WIDTH / scale + 30);
		n += (i / 2) * (VIDEO_HEIGHT / scale + 30);
		cvSetImageROI(DispImage, cvRect(m, n, (int)(VIDEO_WIDTH / scale), (int)(VIDEO_HEIGHT / scale)));
		cvResize(server[i].getImage().src, DispImage);
		cvResetImageROI(DispImage);
	}

	cvShowImage("image", DispImage);
	//cvWaitKey(0);
	cvReleaseImage(&DispImage);
}

void showSaveFiles(SERVER server[]) {
	for (int i = 0; i < CAMERANUM; ++i) {
		server[i].readSaveImage();
	}
	cvShowManyImages(server);
}

int main(void) {
	SERVER server[CAMERANUM];
	bool flag;
	string command;

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
	cout << "save picture: p \t save video: v-123" << endl;
	while (true) {
		cin >> command;
		for (int i = 0; i < CAMERANUM; ++i) {
			server[i].sendMessage(command.c_str());
		}
		if (command == "y") {
			do {
				flag = true;
				for (int i = 0; i < CAMERANUM; ++i) {
					flag = flag & server[i].getFileState();
				}
			}while (!flag); 
			showSaveFiles(server);
		}
	}
	return 0;
}