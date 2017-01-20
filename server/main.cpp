#include <iostream>
#include <string>
#include "server.h"
#include "image.h"

#define PORT 44960
#define IP_ADDRESS "192.168.1.100"
#define CAMERANUM 1

using namespace std;

int main(void) {
	SERVER server[CAMERANUM];
	bool flag;
	string command;
	HANDLE hThread = NULL;

	for (int i = 0; i < CAMERANUM; ++i) {
		flag = server[i].initServer(PORT + i, IP_ADDRESS, i);
		if (!flag) {
			cout << "creat server failed" << endl;
			return -1;
		}
		hThread = CreateThread(NULL, 0, &SERVER :: recieveThread, &server[i], 0, NULL);
		if (hThread == NULL) {
			cout << "creat thread failed" << endl;
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
	}
	return 0;
}