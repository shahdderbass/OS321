/*
 * Shahd Derbass
 * ID: 2231172085
 * Lab08
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

// Global shared variable (ONLY one allowed)
int balance = 1500;

// Mutex for synchronization
pthread_mutex_t lock;

// Function to generate random amount (10 → 250)
int randomAmount() {
	return (rand() % 241) + 10;
}

// Deposit thread function
void* deposit(void* arg){
	int count = *(int*)arg;

    	for (int i = 1; i <= count; i++) {

        	int amount = randomAmount();

        	pthread_mutex_lock(&lock);

        	balance += amount;

        	printf("[Deposit] #%d | +%d KD | SUCCESS | Balance = %d\n", i, amount, balance);

        	pthread_mutex_unlock(&lock);

        	sleep(4);
    	}

    	return NULL;
}

// Withdraw thread function
void* withdraw(void* arg) {
    	int count = *(int*)arg;
	
	for (int i = 1; i <= count; i++) {

        	int amount = randomAmount();
	
        	pthread_mutex_lock(&lock);

        	if (balance >= amount) {
            		balance -= amount;
            		printf("[Withdraw] #%d | -%d KD | SUCCESS | Balance = %d\n", i, amount, balance);
        	} else {
            		printf("[Withdraw] #%d | -%d KD | FAILED | Balance = %d\n", i, amount, balance);
        	}

        	pthread_mutex_unlock(&lock);

        	sleep(2);
    	}

    	return NULL;
}

int main(int argc, char* argv[]) {

    	if (argc != 3) {
        	printf("Usage: %s <withdraw_count> <deposit_count>\n", argv[0]);
        	return 1;
    	}

    	int withdraw_count = atoi(argv[1]);
    	int deposit_count = atoi(argv[2]);

    	pthread_t withdraw_thread, deposit_thread;

    	srand(time(NULL));

    	pthread_mutex_init(&lock, NULL);

    	// Create threads
    	pthread_create(&withdraw_thread, NULL, withdraw, &withdraw_count);
    	pthread_create(&deposit_thread, NULL, deposit, &deposit_count);

    	// Wait for threads to finish
    	pthread_join(withdraw_thread, NULL);
    	pthread_join(deposit_thread, NULL);

    	printf("\nFinal Balance = %d KD\n", balance);

    	pthread_mutex_destroy(&lock);

    	return 0;
}
