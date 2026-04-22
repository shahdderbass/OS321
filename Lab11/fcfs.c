/*
 * Shahd Derbass
 * ID: 2231172085
 * Lab11
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    	char pid[10];
    	int bt, at, priority;
    	int ct, tat, wt;
} Process;

void sortByArrival(Process p[], int n) {
    	for (int i = 0; i < n - 1; i++) {
        	for (int j = 0; j < n - i - 1; j++) {
            		if (p[j].at > p[j + 1].at) {
                		Process temp = p[j];
                		p[j] = p[j + 1];
                		p[j + 1] = temp;
            		}
        	}
    	}
}

int main() {
    	FILE *fp = fopen("input.txt", "r");
    	if (fp == NULL) {
        	printf("Error opening file\n");
        	return 1;
    	}

    	int n;
    	fscanf(fp, "%d", &n);

    	Process *p = (Process *)malloc(n * sizeof(Process));

    	for (int i = 0; i < n; i++) {
        	fscanf(fp, "%s %d %d %d",
               		p[i].pid,
               		&p[i].bt,
               		&p[i].at,
               		&p[i].priority);
    	}

    	fclose(fp);

    	sortByArrival(p, n);

    	int time = 0;
    	float totalWT = 0, totalTAT = 0;

    	for (int i = 0; i < n; i++) {
        	if (time < p[i].at)
            		time = p[i].at;

        	time += p[i].bt;
	
       		p[i].ct = time;
      		p[i].tat = p[i].ct - p[i].at;
       		p[i].wt = p[i].tat - p[i].bt;

        	totalWT += p[i].wt;
        	totalTAT += p[i].tat;
    	}

    	printf("=== FCFS Scheduling ===\n\n");
	
	printf("Gantt Chart:\n");

	// top line (processes)
	for (int i = 0; i < n; i++) {
    		printf("|  %s  ", p[i].pid);
	}
	printf("|\n");

	// bottom line (times)
	printf("0");
	for (int i = 0; i < n; i++) {
    		printf("     %d", p[i].ct);
	}
	printf("\n\n");
    	printf("\n\nProcess\tAT\tBT\tPriority\tCT\tTAT\tWT\n");

    	for (int i = 0; i < n; i++) {
        	printf("%s\t%d\t%d\t%d\t\t%d\t%d\t%d\n",
               		p[i].pid,
               		p[i].at,
               		p[i].bt,
               		p[i].priority,
               		p[i].ct,
               		p[i].tat,
               		p[i].wt);
    	}

    	printf("\nAverage Waiting Time = %.2f\n", totalWT / n);
    	printf("Average Turnaround Time = %.2f\n", totalTAT / n);

    	free(p);
    	return 0;
}
