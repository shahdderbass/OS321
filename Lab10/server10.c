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
#include <pthread.h>
#include <dirent.h>

#define PORT 8080
#define BUFFER_SIZE 1024

pthread_mutex_t file_mutex;

// THREAD FUNCTION
void *handle_client(void *arg) {
    	int *client_socket_ptr = (int *)arg;
    	int client_socket = *client_socket_ptr;
    	free(client_socket_ptr); // dynamic free

    	char *buffer = (char *)malloc(BUFFER_SIZE);
    	char *response = (char *)malloc(BUFFER_SIZE * 2);

    	while (1) {
        	memset(buffer, 0, BUFFER_SIZE);
        	int bytes = recv(client_socket, buffer, BUFFER_SIZE, 0);

        	if (bytes <= 0) break;

        	buffer[strcspn(buffer, "\n")] = 0;

        	// CREATE
        	if (strncmp(buffer, "create ", 7) == 0) {
            		char *filename = (char *)malloc(256);
            		sscanf(buffer + 7, "%s", filename);

            		char *path = (char *)malloc(300);
            		sprintf(path, "shared_files/%s", filename);

            		pthread_mutex_lock(&file_mutex);

            		FILE *fp = fopen(path, "w");
            		if (fp) {
                		fclose(fp);
                		strcpy(response, "File created successfully\n");
            		} 
			else {
                		strcpy(response, "Failed to create file\n");
            		}

           		pthread_mutex_unlock(&file_mutex);

            		send(client_socket, response, strlen(response), 0);

            		free(filename);
            		free(path);
        	}
		
		/*
        	// READ
        	else if (strncmp(buffer, "read ", 5) == 0) {
            		char *filename = (char *)malloc(256);
            		sscanf(buffer + 5, "%s", filename);

            		char *path = (char *)malloc(300);
            		sprintf(path, "shared_files/%s", filename);

            		pthread_mutex_lock(&file_mutex);

            		FILE *fp = fopen(path, "r");
            		memset(response, 0, BUFFER_SIZE * 2);

			if (fp) {
                		fread(response, 1, BUFFER_SIZE * 2 - 1, fp);
                		fclose(fp);
            		} 
			else {
                		strcpy(response, "File not found\n");
            		}

           		pthread_mutex_unlock(&file_mutex);

            		send(client_socket, response, strlen(response), 0);

            		free(filename);
            		free(path);
        	}
		*/

		// READ
		else if (strncmp(buffer, "read ", 5) == 0) {
    			char *filename = (char *)malloc(256);
    			sscanf(buffer + 5, "%s", filename);

    			char *path = (char *)malloc(300);
    			sprintf(path, "shared_files/%s", filename);

    			pthread_mutex_lock(&file_mutex);

    			FILE *fp = fopen(path, "r");
    			memset(response, 0, BUFFER_SIZE * 2);

   			if (fp == NULL) {
        			strcpy(response, "File not found\n");
    			} 
			else {
        			size_t n = fread(response, 1, BUFFER_SIZE * 2 - 1, fp);
        			response[n] = '\0';

        			if (n == 0) {
            				strcpy(response, "File is empty\n");
        			}

        			fclose(fp);
    			}

    			pthread_mutex_unlock(&file_mutex);
	
    			send(client_socket, response, strlen(response), 0);

    			free(filename);
    			free(path);
		}

        	// WRITE
        	else if (strncmp(buffer, "write ", 6) == 0) {
            		char *filename = (char *)malloc(256);
            		char *data = (char *)malloc(700);

            		if (sscanf(buffer + 6, "%s %[^\n]", filename, data) >= 2) {
                		char *path = (char *)malloc(300);
                		sprintf(path, "shared_files/%s", filename);

                		pthread_mutex_lock(&file_mutex);

                		FILE *fp = fopen(path, "a");
                		if (fp) {
                    			fprintf(fp, "%s", data);
                    			fclose(fp);
                    			strcpy(response, "Write successful\n");
                		} 
				else {
                    			strcpy(response, "Write failed\n");
                		}

                		pthread_mutex_unlock(&file_mutex);

                		free(path);
            		} 
			else {
                		strcpy(response, "Invalid write command\n");
            		}

            		send(client_socket, response, strlen(response), 0);

            		free(filename);
            		free(data);
        	}

        	// LIST
        	else if (strcmp(buffer, "list") == 0) {
            		DIR *dir;
            		struct dirent *entry;

            		memset(response, 0, BUFFER_SIZE * 2);

            		pthread_mutex_lock(&file_mutex);

            		dir = opendir("shared_files");
            		if (dir) {
                		while ((entry = readdir(dir)) != NULL) {
                    			if (strcmp(entry->d_name, ".") != 0 &&
                        		   strcmp(entry->d_name, "..") != 0) {
                        		   strcat(response, entry->d_name);
                        			strcat(response, "\n");
                    			}
                		}
                		closedir(dir);

                		if (strlen(response) == 0)
                    			strcpy(response, "No files available\n");
            		} 
			else {
                		strcpy(response, "Directory error\n");
            		}

            			pthread_mutex_unlock(&file_mutex);
	
            			send(client_socket, response, strlen(response), 0);
        	}

        	// QUIT
        	else if (strcmp(buffer, "quit") == 0) {
            		strcpy(response, "Goodbye\n");
            		send(client_socket, response, strlen(response), 0);
            		break;
        	}

        	// INVALID
        	else {
            		strcpy(response, "Invalid command\n");
            		send(client_socket, response, strlen(response), 0);
        	}
    	}

    	close(client_socket);
    	free(buffer);
    	free(response);

    	pthread_exit(NULL);
}

// MAIN
int main() {
    	int *server_socket = (int *)malloc(sizeof(int));

    	struct sockaddr_in *server_addr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));

    	*server_socket = socket(AF_INET, SOCK_STREAM, 0);
    	if (*server_socket < 0) {
        	perror("socket failed");
        	exit(1);
    	}

    	server_addr->sin_family = AF_INET;
    	server_addr->sin_addr.s_addr = INADDR_ANY;
    	server_addr->sin_port = htons(PORT);

    	if (bind(*server_socket, (struct sockaddr *)server_addr,
             sizeof(*server_addr)) < 0) {
        	perror("bind failed");
        	exit(1);
    	}

    	if (listen(*server_socket, 5) < 0) {
        	perror("listen failed");
        	exit(1);
    	}

    	pthread_mutex_init(&file_mutex, NULL);

    	printf("Server running on port %d...\n", PORT);

    	while (1) {
        	int *client_socket = (int *)malloc(sizeof(int));
        	struct sockaddr_in client_addr;
        	socklen_t len = sizeof(client_addr);

        	*client_socket = accept(*server_socket,
                                (struct sockaddr *)&client_addr, &len);

        	if (*client_socket < 0) {
            		perror("accept failed");
            		free(client_socket);
            		continue;
        	}

        	pthread_t tid;
        	pthread_create(&tid, NULL, handle_client, client_socket);
        	pthread_detach(tid);
    	}

    	close(*server_socket);
    	free(server_socket);
    	free(server_addr);

    	pthread_mutex_destroy(&file_mutex);

    	return 0;
}
