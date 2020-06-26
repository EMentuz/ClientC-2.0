
///////////////////////////////////////клиент 1
#include "pch.h"
#include <iostream>
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#pragma warning(disable: 4996)
#include <opencv2/opencv.hpp>
#include <opencv/cv.hpp>

#include <opencv2/highgui.hpp>
#include <stdlib.h>

#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;
#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480
Mat cameraFeed;
int IM_HEIGHT, IM_WIDTH, imgSize;


SOCKET Connection;

//принимаем сообщения в клиенте
void ClientHandler() {
	int msg_size;
	while (true) {
		recv(Connection, (char*)&msg_size, sizeof(int), NULL);
		char *msg = new char[msg_size + 1];
		msg[msg_size] = '\0';
		recv(Connection, msg, msg_size, NULL);
		std::cout << msg << std::endl;
		delete[] msg;
	}
}

int main(int argc, char* argv[]) {
	//WSAStartup
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		std::cout << "Error" << std::endl;
		exit(1);
	}

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	char ip[10]; std::cout << "Enter the ip address: "; std::cin >> ip;
	addr.sin_addr.s_addr = inet_addr(ip);
	int port; std::cout << "Enter the server port: "; std::cin >> port;
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;

	Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
		std::cout << "Error: failed connect to server.\n";
		return 1;
	}
	std::cout << "Connected!\n";

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, NULL, NULL, NULL);

	std::string msg1;

	VideoCapture cap(0);

	// if not success, exit program
	if (cap.isOpened() == false)
	{
		std::cout << "Cannot open the video camera\n";
		std::cin.get(); //wait for any key press
		return -1;
	}

	while (true) {


		if (!cap.read(cameraFeed))
		{
			std::cout << "Video ended";
			break;
		}

		IM_HEIGHT = FRAME_HEIGHT;
		IM_WIDTH = FRAME_WIDTH;
		resize(cameraFeed, cameraFeed, Size(IM_WIDTH, IM_HEIGHT));
		imgSize = cameraFeed.total()*cameraFeed.elemSize();
		std::cout << "Size:  " << imgSize << std::endl;
		send(Connection, reinterpret_cast<const char*>(cameraFeed.data), imgSize, NULL);

		namedWindow("Camera");
		imshow("Camera", cameraFeed);
		//std::getline(std::cin, msg1);
		//int msg_size = msg1.size();
		//send(Connection, (char*)&msg_size, sizeof(int), NULL);
		//send(Connection, msg1.c_str(), msg_size, NULL);

		if (waitKey(30) == 27)
	    break;
	
	}
		 
	closesocket(Connection);

	system("pause");
	return 0;
}

