/*
 * Shahd Derbass
 * ID: 2231172085
 * Lab09
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 100

typedef struct {
    	int total_inodes;
    	int total_blocks;
    	int status;
} superblock;

typedef struct {
    	char name[8];
    	int size;
    	int blocks[100];   // pointers/indexes to disk blocks
	int block_count;
	int used;
} inode;

typedef struct {
	char data[BLOCK_SIZE];
	int used;
} disk_block;

superblock *sb;
inode *inodes;
disk_block *blocks;

void create_fs() {

    	// allocate superblock
    	sb = (superblock *)malloc(sizeof(superblock));

    	// set superblock info
    	sb->total_inodes = 10;
    	sb->total_blocks = 100;
    	sb->status = 1;

    	// allocate memory
    	inodes = (inode *)malloc(10 * sizeof(inode));
    	blocks = (disk_block *)malloc(100 * sizeof(disk_block));

    	// check allocation
    	if (sb == NULL || inodes == NULL || blocks == NULL) {
        	printf("Error: Memory allocation failed\n");
        	exit(1);
    	}

    	// initialize inodes
    	for (int i = 0; i < 10; i++) {
        	inodes[i].used = 0;
        	inodes[i].size = 0;
        	inodes[i].block_count = 0;

        	for (int j = 0; j < 100; j++) {
            		inodes[i].blocks[j] = -1;
        	}
    	}

    	// initialize disk blocks
    	for (int i = 0; i < 100; i++) {
        	blocks[i].used = 0;
        	memset(blocks[i].data, 0, BLOCK_SIZE);
    	}

    	printf("File system created successfully\n");
}

void sync_fs() {
    	FILE *fp;

    	fp = fopen("file_data", "wb");
    	if (fp == NULL) {
        	printf("Error: Could not open file\n");
        	return;
    	}

    	// write superblock
    	fwrite(sb, sizeof(superblock), 1, fp);

    	// write inodes
    	fwrite(inodes, sizeof(inode), 10, fp);

    	// write disk blocks
    	fwrite(blocks, sizeof(disk_block), 100, fp);

    	fclose(fp);

    	printf("File system saved to disk (file_data)\n");
}

void print_fs() {

    	// ===== FILE SYSTEM INFO =====
    	printf("===== FILE SYSTEM INFO =====\n");
    	printf("Number of inodes: %d\n", sb->total_inodes);
    	printf("Number of blocks: %d\n", sb->total_blocks);

    	printf("\n");

    	// ===== INODES =====
    	printf("===== INODES =====\n");

    	for (int i = 0; i < 10; i++) {

        	if (inodes[i].used == 0) {
            		printf("[%d] Name: EMPTY\n", i);
        	}
        	else {
            		printf("[%d] Name: %s Size: %d Blocks: %d\n",
                   		i,
                   		inodes[i].name,
                   		inodes[i].size,
                   		inodes[i].block_count);
        	}
    	}

    	printf("\n");

    	// ===== BLOCK USAGE =====
    	printf("===== BLOCK USAGE =====\n");

    	printf("Used blocks: ");
    	for (int i = 0; i < 100; i++) {
        	if (blocks[i].used == 1) {
            		printf("%d ", i);
        	}
    	}
    	printf("\n");

    	printf("Free blocks: ");
    	for (int i = 0; i < 100; i++) {
        	if (blocks[i].used == 0) {
            		printf("%d ", i);
        	}
    	}
    	printf("\n");
}

void mount_fs() {
    	FILE *fp;

    	fp = fopen("file_data", "rb");
    	if (fp == NULL) {
        	printf("Error: Could not open file_data\n");
        	return;
    	}

    	// allocate superblock
    	sb = (superblock *)malloc(sizeof(superblock));

    	// read superblock 
    	fread(sb, sizeof(superblock), 1, fp);

    	// allocate memory
    	inodes = (inode *)malloc(10 * sizeof(inode));
    	blocks = (disk_block *)malloc(100 * sizeof(disk_block));

    	if (sb == NULL || inodes == NULL || blocks == NULL) {
        	printf("Error: Memory allocation failed\n");
        	return;
    	}

    	// read inodes
    	fread(inodes, sizeof(inode), 10, fp);

    	// read blocks
    	fread(blocks, sizeof(disk_block), 100, fp);

    	fclose(fp);
	
    	printf("File system mounted successfully\n");
}

int find_empty_node() {
    	for (int i = 0; i < 10; i++) {
        	if (inodes[i].used == 0) {
            		return i;
        	}
    	}
    	return -1;
}

int find_empty_block() {
    	for (int i = 0; i < 100; i++) {
        	if (blocks[i].used == 0) {
            		return i;
        	}
    	}
    	return -1;
}

int allocate_file(char *name) {

    	int index = find_empty_node();

    	if (index == -1) {
        	printf("Error: No free inode available\n");
        	return -1;
    	}

    	// mark inode as used
    	inodes[index].used = 1;

    	// set file name
    	strcpy(inodes[index].name, name);

    	// initialize file
    	inodes[index].size = 0;
    	inodes[index].block_count = 0;

    	for (int i = 0; i < 100; i++) {    
        	inodes[index].blocks[i] = -1;
    	}

    	printf("File allocated: %s\n", name);
    	printf("File descriptor: %d\n", index);

    	return index;
}

void set_filesize(int filenum, int size) {

    	// check valid file
    	if (filenum < 0 || filenum >= 10 || inodes[filenum].used == 0) {
        	printf("Error: invalid file\n");
        	return;
    	}
	
	if (size < 0) {
    		printf("Error: invalid file size\n");
    		return;
	}

    	int blocks_needed = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    	int current_blocks = inodes[filenum].block_count;

    	// need MORE blocks
    	if (blocks_needed > current_blocks) {

        	for (int i = current_blocks; i < blocks_needed; i++) {

            		int b = find_empty_block();

            		if (b == -1) {
                		printf("Error: no free blocks\n");
                		return;
            		}

            		blocks[b].used = 1;
            		inodes[filenum].blocks[i] = b;
        	}
    	}

    	else if (blocks_needed < current_blocks) {

        	for (int i = blocks_needed; i < current_blocks; i++) {
            		int b = inodes[filenum].blocks[i];

            		blocks[b].used = 0;
            		inodes[filenum].blocks[i] = -1;
        	}
    	}

    	// update inode
    	inodes[filenum].block_count = blocks_needed;
    	inodes[filenum].size = size;

    	printf("File %d size updated to %d bytes\n", filenum, size);
    	printf("Blocks allocated: %d\n", blocks_needed);
}

void write_byte(int filenum, int pos, char *data) {

    	// check valid
    	if (filenum < 0 || filenum >= 10 || inodes[filenum].used == 0) {
        	printf("Error: invalid file\n");
        	return;
    	}

    	if (pos >= inodes[filenum].size) {
        	printf("Error: write beyond file size\n");
        	return;
    	}

    	// find block index
    	int block_index = pos / BLOCK_SIZE;

    	// find actual block number
    	int block_num = inodes[filenum].blocks[block_index];
	
	if (block_num == -1) {
    		printf("Error: invalid block access\n");
    		return;
	}
    	// offset inside block
    	int offset = pos % BLOCK_SIZE;

    	// write data 
    	blocks[block_num].data[offset] = data[0];

    	printf("Data written successfully to file %d\n", filenum);
}

int main() {
    	
	// create + save
	create_fs();
 	sync_fs();
    	
	// load system
//	mount_fs();
	
	// create multiple files
	int f1 = allocate_file("first");
	int f2 = allocate_file("second");
	
	// set sizes
	set_filesize(f1, 5000); 
	set_filesize(f2, 5000);

	// write data	
	write_byte(f1, 0, "S");
	write_byte(f2, 0, "D");
	
	// save again
    	sync_fs();

	// reload
	mount_fs();
	
	// print final state
	print_fs();
	//printf("done\n");
    	return 0;
}
