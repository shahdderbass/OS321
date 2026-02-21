#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

struct bg_ps {
	pid_t pid;
	char* cmd;
	struct bg_ps *next;
};

int main(){
	char* buf = malloc(1024);
	if(!buf){
		perror("Failed Memory Allocation.");
		return -1;
	}
	char **argv = malloc(64*sizeof(char*));
	if(!argv){
		perror("Failed Memory Allocation.");
		return -1;
	}
	struct bg_ps *bg_head = NULL;
	
	char *mylog[3] = {NULL, NULL, NULL};
	int log_count = 0;
	
	int bg_flag = 0;

	int status;
	pid_t done;
	
	printf("ShahdShell> ");
	while(fgets(buf, 1024, stdin)){
		bg_flag = 0;		
		
		while ((done = waitpid(-1, &status, WNOHANG)) > 0) {
			struct bg_ps *curr = bg_head;
            		struct bg_ps *prev = NULL;

           		while (curr) {
                		if (curr->pid == done) {
                    			if (prev)
                        			prev->next = curr->next;
                    			else
                        			bg_head = curr->next;

                    			free(curr->cmd);
                    			free(curr);
                    			break;
                		}
                		prev = curr;
                		curr = curr->next;
            		}
		}

		buf[strcspn(buf, "\n")] = '\0';
		
		int only_spaces = 1;

		for (int k = 0; buf[k] != '\0'; k++) {
    			if (buf[k] != ' ' && buf[k] != '\t') {
        			only_spaces = 0;
        			break;
    			}
		}

		if (only_spaces) {
    			printf("ShahdShell> ");
    			continue;
		}
		
		/*	
		// ignore empty input 
        	if (strlen(buf) == 0) {
            		printf("ShahdShell> ");
            		continue;
        	}
		*/
		/* & alone */
		if(strcmp(buf,"&") == 0){
			printf("Error: missing command\n");
			printf("ShahdShell> ");
         		continue;
        	}
		
		// making a copy of the command line before strtok modifies it	
		char *line_copy = malloc(strlen(buf) + 1);
		if (!line_copy) {
    			perror("Failed Memory Allocation.");
    			// free any argv from previous iteration if needed (usually none here)
    			printf("ShahdShell> ");
    			continue;
		}
		strcpy(line_copy, buf);

		// tokenize
		char* token = strtok(buf, " \t");
		int i= 0;
		while(token != NULL && i < 63){
			argv[i] = malloc(strlen(token)+1);
			strcpy(argv[i], token);
			i++;
			token = strtok(NULL, " \t");
		}

		argv[i] = NULL;
		
		if (i > 0 && strcmp(argv[i-1], "&") == 0) {
    			free(argv[i-1]);     // free "&"
    			argv[i-1] = NULL;    // remove it
    			bg_flag = 1;
		}

		if (strcmp(argv[0], "myexit") == 0) {

            		/* kill all background */
           		struct bg_ps *curr = bg_head;
            		while (curr) {
                		kill(curr->pid, SIGTERM);
                		curr = curr->next;
           		}

		        /* reap them */
            		while (waitpid(-1, &status, 0) > 0);

            		/* free bg list */
            		while (bg_head) {
                		struct bg_ps *tmp = bg_head;
                		bg_head = bg_head->next;
                		free(tmp->cmd);
                		free(tmp);
            		}

            		/* free log */
            		for (int j=0; j<log_count; j++)
                		free(mylog[j]);

            		/* free argv memory */
            		for (int j=0; j<i; j++)
                		free(argv[j]);

            		free(argv);
            		free(buf);
            		exit(0);
        	}
		
		if(strcmp(argv[0], "mylog")==0){
    			for(int j = 0; j < log_count; j++)
        			printf("%s\n", mylog[j]);

    			free(line_copy);
	
    			for(int j = 0; j < i; j++)
				free(argv[j]);

    			printf("ShahdShell> ");
  			 continue;
		}

		if (log_count == 3) {
        		free(mylog[2]);
       			mylog[2] = mylog[1];
       			mylog[1] = mylog[0];
       			mylog[0] = line_copy;
		} 
		else {
        		for (int t = log_count; t > 0; t--)
            			mylog[t] = mylog[t-1];
        		mylog[0] = line_copy;
       			log_count++;
  		}

		if(strcmp(argv[0], "myprocess") == 0){
			if(!bg_head)
				printf("No background processes\n");

			else {
				printf("PID COMMAND\n");
				struct bg_ps *curr = bg_head;
				while(curr){
					printf("%d %s\n", curr->pid, curr->cmd);
					curr = curr->next;
				}	
			}

			for(int j = 0; j <i; j++)
				free(argv[j]);

			printf("ShahdShell> ");
			continue;
		}	


		// fork		
	       	pid_t pid = fork();	
		if(pid == 0){
			char* path = malloc(256);
			if(!path){
				perror("Failed Memory Allocation");
				return -1;
			}
			
			snprintf(path, 256, "./%s", argv[0]);
    			execv(path, argv);

    			snprintf(path, 256, "/bin/%s", argv[0]);
    			execv(path, argv);

    			fprintf(stderr,"Error: command not found\n");
    			free(path);
			exit(1);
		
		}
		else if(pid > 0){
		       if(!bg_flag)	
				waitpid(pid, &status, 0);
			else {
                		struct bg_ps *node = malloc(sizeof(struct bg_ps));
                		node->pid = pid;

                		node->cmd = malloc(1024);
                		node->cmd[0] = '\0';

                		for (int k=0; argv[k]; k++) {
                    			strcat(node->cmd, argv[k]);
                    			strcat(node->cmd, " ");
                		}

                		node->next = bg_head;
                		bg_head = node;
            		}
		}
		else
			perror("Fork error");
				
		for(int j = 0; j < i; j++)
			free(argv[j]);

		printf("ShahdShell> ");
	}

	return 0;
}
