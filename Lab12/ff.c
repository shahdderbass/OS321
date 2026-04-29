#include <stdio.h>
#include <stdlib.h>

int main() {
        FILE *fp = fopen("ff_input.txt", "r");
        if (fp == NULL) {
            printf("Error opening file.\n");
            return 1;
        }

        int nBlocks, nFiles;

        fscanf(fp, "%d", &nBlocks);

        int *blocks = (int*) malloc(nBlocks * sizeof(int));
        int *allocated = (int*) calloc(nBlocks, sizeof(int));

        for (int i = 0; i < nBlocks; i++) {
            fscanf(fp, "%d", &blocks[i]);
        }

        fscanf(fp, "%d", &nFiles);

        int *files = (int*) malloc(nFiles * sizeof(int));

        for (int i = 0; i < nFiles; i++) {
            fscanf(fp, "%d", &files[i]);
        }

        fclose(fp);

        printf("First Fit Allocation:\n");

        for (int i = 0; i < nFiles; i++) {
            int flag = 0;

            for (int j = 0; j < nBlocks; j++) {
                if (!allocated[j] && blocks[j] >= files[i]) {
                    printf("File %d (size %d) -> Block %d (size %d)\n",
                        i+1, files[i], j+1, blocks[j]);
                    allocated[j] = 1;
                    flag = 1;
                    break;
                }
            }

            if (!flag) {
                printf("File %d (size %d) -> Not Allocated\n",
                    i+1, files[i]);
            }
        }

        free(blocks);
        free(files);
        free(allocated);

        return 0;
}
