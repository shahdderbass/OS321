#include <stdio.h> 
#include <unistd.h> 
 
int main() {     
	printf("Process ID: %d\n", getpid());
   	sleep(500);
   	return 0; 

} 

