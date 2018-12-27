#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <omp.h>
#include <pthread.h>
#include <semaphore.h>
#define sleep_time 0.1

int phi_cnt;  /*count of philosophers*/
#define left_fork (idx % phi_cnt)
#define right_fork ((idx+1) % phi_cnt)
char* method;
sem_t forks[55];

// ---------------------------normal-----------------------------------
// to see how deadlock happen, I do some print
void wait_left_fork(long idx){
    sem_wait(&forks[left_fork]);
    printf("\tphilosopher %ld: get fork(%ld).\n", idx, left_fork);
}
void wait_right_fork(long idx){
    sem_wait(&forks[right_fork]);
    printf("\tphilosopher %ld: get fork(%ld).\n", idx, right_fork);
}
void free_left_fork(long idx){
    sem_post(&forks[left_fork]);
    printf("\tphilosopher %ld: free fork(%ld).\n", idx, left_fork);
}
void free_right_fork(long idx){
    sem_post(&forks[right_fork]);
    printf("\tphilosopher %ld: free fork(%ld).\n", idx, right_fork);
}

// may cause deadlock
void philosopher_normal(long idx){
    while(1){
        printf("philosopher %ld: thinking.\n", idx);
        sleep(sleep_time);  /* thinking */
        printf("philosopher %ld: trying.\n", idx);
        wait_left_fork(idx);
        wait_right_fork(idx);
        printf("philosopher %ld: eating.\n", idx);
        sleep(sleep_time); /* eating */
        free_left_fork(idx);
        free_right_fork(idx);
    }
}

// ----------------------method1: Arbitrator solution---------------------
//guarantee that a philosopher can only pick up both forks or none by introducing an arbitrator
void wait_2_forks(long idx){
    sem_trywait(&forks[left_fork]);
    sem_trywait(&forks[right_fork]);
}

// put down 2 forks at the same time
void free_2_forks(long idx){
    sem_post(&forks[left_fork]);
    sem_post(&forks[right_fork]);
}

void philosopher_method1(long idx){
    while(1){
        printf("phisolopher: %ld: thinking.\n", idx);
        sleep(sleep_time);
        printf("phisolopher: %ld: trying.\n", idx);
        wait_2_forks(idx);
        printf("phisolopher: %ld: eating.\n", idx);
        sleep(sleep_time);
        free_2_forks(idx);
    }
}

// ----------------method2: Resource hierarchy solution-------------------
/**
 * assigns a partial order to the resources (the forks, in this case), 
 * and establishes the convention that all resources will be requested in order
*/
#define lower_fork (idx+1==phi_cnt ? 0 : idx)
#define upper_fork (idx+1==phi_cnt ? idx : idx+1)

void wait_lower_fork(long idx){
    sem_wait(&forks[lower_fork]);
}
void wait_upper_fork(long idx){
    sem_wait(&forks[upper_fork]);
}
void free_lower_fork(long idx){
    sem_post(&forks[lower_fork]);
}
void free_upper_fork(long idx){
    sem_post(&forks[upper_fork]);
}

void philosopher_method2(long idx){
    while(1){
        printf("philosopher %ld: thinking.\n", idx);
        sleep(sleep_time);  /* thinking */
        printf("philosopher %ld: trying.\n", idx);
        wait_lower_fork(idx);
        wait_upper_fork(idx);
        printf("philosopher %ld: eating.\n", idx);
        sleep(sleep_time); /* eating */
        free_lower_fork(idx);
        free_upper_fork(idx);
    }
}

// -------------------------boy next door------------------------------

void* dark_deep_fantasy(){
    long rnk = (int)omp_get_thread_num();

    if (strcmp(method, "-normal") == 0){
        philosopher_normal(rnk);
    } else if (strcmp(method, "-method1") == 0){
        philosopher_method1(rnk);
    } else if (strcmp(method, "-method2") == 0){
        philosopher_method2(rnk);
    }
    return NULL;
}

int main(int argc, char* argv[]){
    // get variates from command
    method = argv[1];
    sscanf(argv[3], "%d", &phi_cnt);

    // init forks (semaphore)
    for (int i = 0; i < phi_cnt; i++){
        sem_init(&forks[i], 0, 0);
        sem_post(&forks[i]);
    } 

    // init philosophers (pthread), and then begin ♂
    #pragma omp parallel num_threads(phi_cnt)
    dark_deep_fantasy();

    return 0;
}
