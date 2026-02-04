/*
 * Shahd Derbass
 * 2231172085
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>

int main(int argc, char **argv){
	
	if(argc != 2){
		perror("Invalid number of arguments.");
		exit(1);
	}

	int fd = open(argv[1],O_RDONLY);
	if(fd < 0){
		perror("Error opening file.");
		exit(1);
	}
	pid_t pid = fork();
	if (pid < 0) {
        	perror("fork");
        	exit(1);
    	}

	if(pid == 0){
		sleep(5);

		printf("Child Process ID: %d\n", getpid());
			
		char buf[1024];
		ssize_t r;
		while ((r = read(fd, buf, sizeof(buf))) > 0) {
    			write(STDOUT_FILENO, buf, r);
		}
		if (r < 0) {
    			perror("read");
    			exit(1);
		}
	

		exit(0);
	}
	else{
		if (wait(NULL) < 0) {
            		perror("wait");
            		close(fd);
            		exit(1);
        	}

		printf("Parent Process ID: %d\n", getpid());
		
		if(close(fd) < 0){
			perror("Error closing file.");
			exit(1);
		}

		printf("done\n");

		exit(0);
	}

	
	return 0;
}

