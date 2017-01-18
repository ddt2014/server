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
	bool flag = false;
	string command;

	for (int i = 0; i < CAMERANUM; ++i) {
		server[i].initServer(PORT + i, IP_ADDRESS);
	}
	while (!flag) {
		flag = true;
		for (int i = 0; i < CAMERANUM; ++i)
			flag = flag & server[i].getState();
	}
	cout << "input" << endl;
	while (true) {
		cin >> command;
		for (int i = 0; i < CAMERANUM; ++i) {
			server[i].sendMessage(command.c_str());
		}
	}
	dfas
	return 0;
}sdfsd