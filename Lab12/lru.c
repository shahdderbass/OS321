#include <stdio.h>
#include <stdlib.h>

int findLRU(int time[], int n) {
        int min = time[0], pos = 0;
        for (int i = 1; i < n; i++) {
            if (time[i] < min) {
                min = time[i];
                pos = i;
            }
        }
        return pos;
    }

    int main() {
        FILE *fp = fopen("lru_input.txt", "r");
        if (fp == NULL) {
            printf("Error opening file.\n");
            return 1;
        }

        int n, frames_count;

        fscanf(fp, "%d", &n);

        int *pages = (int*) malloc(n * sizeof(int));

        for (int i = 0; i < n; i++) {
            fscanf(fp, "%d", &pages[i]);
        }

        fscanf(fp, "%d", &frames_count);

        fclose(fp);

        int *frames = (int*) malloc(frames_count * sizeof(int));
        int *time = (int*) malloc(frames_count * sizeof(int));

        for (int i = 0; i < frames_count; i++) {
            frames[i] = -1;
        }

        int counter = 0, faults = 0;

        for (int i = 0; i < n; i++) {
            int found = 0;

            for (int j = 0; j < frames_count; j++) {
                if (frames[j] == pages[i]) {
                    counter++;
                    time[j] = counter;
                    found = 1;
                    break;
                }
            }

            if (!found) {
                int pos = -1;

                for (int j = 0; j < frames_count; j++) {
                    if (frames[j] == -1) {
                        pos = j;
                        break;
                    }
                }

                if (pos == -1) {
                    pos = findLRU(time, frames_count);
                }

                frames[pos] = pages[i];
                counter++;
                time[pos] = counter;
                faults++;
            }
        }

        printf("Total Page Faults = %d\n", faults);

        free(pages);
        free(frames);
        free(time);

        return 0;
}
