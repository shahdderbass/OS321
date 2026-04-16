/*
 * Shahd Derbass
 * ID: 2231172085
 * Lab10
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    	int *sock = (int *)malloc(sizeof(int));

    	struct sockaddr_in *server_addr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));

    	char *buffer = (char *)malloc(BUFFER_SIZE);
    	char *response = (char *)malloc(BUFFER_SIZE * 2);

    	*sock = socket(AF_INET, SOCK_STREAM, 0);
    	if (*sock < 0) {
        	perror("socket failed");
        	exit(1);
    	}

    	server_addr->sin_family = AF_INET;
    	server_addr->sin_port = htons(PORT);

    	printf("Enter server IP: ");
    	char *ip = (char *)malloc(50);
    	scanf("%s", ip);
    	getchar();

    	if (inet_pton(AF_INET, ip, &server_addr->sin_addr) <= 0) {
        	perror("invalid address");
        	exit(1);
    	}

    	if (connect(*sock, (struct sockaddr *)server_addr,
                sizeof(*server_addr)) < 0) {
        	perror("connect failed");
        	exit(1);
    	}

    	printf("Connected to server\n");

    	while (1) {
        	printf("Enter command: ");
        	fgets(buffer, BUFFER_SIZE, stdin);

        	send(*sock, buffer, strlen(buffer), 0);

        	memset(response, 0, BUFFER_SIZE * 2);
        	int bytes = recv(*sock, response, BUFFER_SIZE * 2, 0);

        	if (bytes <= 0) break;

        	printf("Server: %s\n", response);

        	if (strncmp(buffer, "quit", 4) == 0)
            		break;
    	}

    	close(*sock);

    	free(sock);
    	free(server_addr);
    	free(buffer);
    	free(response);
    	free(ip);

    	return 0;
}
