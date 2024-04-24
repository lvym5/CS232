/*
 * Name: Lily McAboy (lvm5)
 * Date: 3/23/2024
 * CS232-B
 */

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define NUM_VIALS 30
#define SHOTS_PER_VIAL 6
#define NUM_CLIENTS (NUM_VIALS * SHOTS_PER_VIAL)
#define NUM_NURSES 10
#define NUM_STATIONS NUM_NURSES
#define NUM_REGISTRATIONS_SIMULTANEOUSLY 4

/* global variables */

// Here is the mutex information in C that I used to help me: https://www.geeksforgeeks.org/mutex-lock-for-linux-thread-synchronization/ 
// Also used this: https://begriffs.com/posts/2020-03-23-concurrent-programming.html 

int num_vials_left = NUM_VIALS; //initialize the number of vials left to the number of vials available

sem_t vials_mutex; //create a mutex specifically for the vaccines
sem_t waiting_queue_mutex; //create a for the waiting line at registration
sem_t station_mutex; //create a mutex for the station's critical section
sem_t remaining_stations_sem; //semaphore to tell us how many stations are open  
sem_t its_sempty; //hw many stations are not available

//Create bounded buffer
long int stationBuffer[NUM_STATIONS];

int nurseIndex = 0;
int clientIndex = 0;

//create arrays for rendezvous issues
sem_t client_sems[NUM_STATIONS]; //create a client semaphore array for the number of stations
sem_t nurse_sems[NUM_STATIONS]; //create a nurse semaphore array for the number of stations
/* global functions */

int get_rand_in_range(int lower, int upper) {
    return (rand() % (upper - lower + 1)) + lower;
}

char *time_str;
char *curr_time_s() {
    time_t t;
    time(&t);
    time_str = ctime(&t);
    // replace ending newline with end-of-string.
    time_str[strlen(time_str) - 1] = '\0';
    return time_str;
}

// lower and upper are in seconds.
void walk(int lower, int upper) {
    usleep(get_rand_in_range(lower * 1000000, upper * 1000000)); //usleep uses micro-seconds, so we have to multiply the bounds by 1,000,000
}

void sleepy(int seconds){
    usleep(seconds * 1000000);
}

// arg is the nurses station number.
//Used this website to help me with the locking and unlocking: https://www.man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html 
void *nurse(void *arg) {
    long int id = (long int)arg;

    /* Algorithm:
    create thread 
    loop 6 times: 
        nurse walks 1-3 secs to get vaccine (if no more vaccines, leave)
        go back to station and take 1-3 seconds
        let client know they're ready to administer vaccine
        client lets them know they are ready
        Nurse gives the vaccination, which takes 5 secs
    */

    fprintf(stderr, "%s: nurse %ld started\n", curr_time_s(), id);

    while(1){
        fprintf(stderr, "%s: nurse %ld is walking to get a vial. \n", curr_time_s(), id);
        walk(1,3); //walk to get vaccine
        sem_wait(&vials_mutex); //lock vials mutex
        if(num_vials_left == 0){
            fprintf(stderr, "%s: nurse %ld sees there are no vials left, so the shift is done!\n", curr_time_s(), id);
            sem_post(&vials_mutex); //if no vials left, unlock the vial mutex and leave
            break;
        }
        num_vials_left--; //decrement the number of vials since we just took one
        fprintf(stderr, "%s: nurse %ld got vial. Number of vials left: %d \n", curr_time_s(), id, num_vials_left);
        sem_post(&vials_mutex);

        walk(1,3); //go back to the station

        for(int i = 0; i < SHOTS_PER_VIAL; i++){
            sem_wait(&its_sempty); //check bouned buffer for emty spots
            sem_wait(&station_mutex); //lock station mutex because nurse is "getting ready"
            stationBuffer[nurseIndex] = id; //add the nurse id to the nurse index in the bounded buffer
            nurseIndex = (nurseIndex + 1) % NUM_STATIONS; // Allow us to utiize a circular bounded buffer
            fprintf(stderr, "%s: nurse %ld is ready for their client\n", curr_time_s(), id); //indicate that the nurse is ready
            sem_post(&station_mutex); //unlock the station mutex because nurse is ready!
            sem_post(&remaining_stations_sem); //tells us that we have a station ready to go
            fprintf(stderr, "%s: nurse %ld waiting for client to arrive.\n", curr_time_s(), id);

            sem_wait(&client_sems[id]); //Nurse waits for client at their station to be ready for a shot
            fprintf(stderr, "%s: nurse %ld is administrating the vaccine \n", curr_time_s(), id); //administer the vaccine
            sleepy(5); //administer vaccination
            sem_post(&nurse_sems[id]); 
            fprintf(stderr, "%s: nurse %ld allows client to leave. \n", curr_time_s(), id); //administer the vaccine
        }
    }

    fprintf(stderr, "%s: nurse %ld leaves the clinic\n", curr_time_s(), id); //Nurse lets their bosses know the nurse is done with their job

    pthread_exit(NULL); //nurse leaves 
}

void *client(void *arg) { //int will be used for id and station
    long int id = (long int)arg;

    /* Algorithm:
    create thread (client)
    Walk to desk (3-10 seconds) why is he so slow??
    wait in line for an opening 
    Register at the desk (3-10 seconds)
    Walk to the station assignment queue (3-10 seconds)
    Wait for the station assignment
    Walk to nurse's station (1-2 seconds)
    Tell the nurse that client is ready for vaccine
    wait for nurse to administer vaccine
    leave clinic 
    */

    //client gets to clinic
    fprintf(stderr, "%s: client %ld has arrived and is walking to register\n", curr_time_s(), id);
    walk(3,10); //walk a random amount of time to the desk

    //Wait in line
    fprintf(stderr, "%s: client %ld is waiting in line to register.\n", curr_time_s(), id);
    sem_wait(&waiting_queue_mutex);
    walk(3,10); 
    fprintf(stderr, "%s: client %ld is registered. \n", curr_time_s(), id);
    sem_post(&waiting_queue_mutex);


    //Walk to nurse station
    fprintf(stderr, "%s: client %ld is walking to the nurse station queue.\n", curr_time_s(), id);
    walk(3,10);    

    sem_wait(&remaining_stations_sem); //Check if there are any open statins for a client to go to
    sem_wait(&station_mutex); //Open up critical section :)
    long int chosenNurse = stationBuffer[clientIndex]; //choose a nurse
    clientIndex = (clientIndex + 1) % NUM_STATIONS;  
    fprintf(stderr, "%s: client %ld aproaches nurse %ld for good times (vaccinations, of course).\n", curr_time_s(), id, chosenNurse);
    sem_post(&station_mutex);
    sem_post(&its_sempty);
    //Tell nurse that we are vaccine ready
    sem_post(&client_sems[chosenNurse]);
    fprintf(stderr, "%s: client %ld is ready for the vaccination from nurse %ld\n", curr_time_s(), id, chosenNurse);
    sem_wait(&nurse_sems[chosenNurse]); //waits for shot
    fprintf(stderr, "%s: client %ld leaves the clinic after getting shot.\n", curr_time_s(), id);

    pthread_exit(NULL);
}

int main() {
    srand(time(0));
    //using pthread_create: https://stackoverflow.com/questions/6990888/c-how-to-create-thread-using-pthread-create-function 

    sem_init(&waiting_queue_mutex, 0, NUM_REGISTRATIONS_SIMULTANEOUSLY);
    sem_init(&vials_mutex, 0, 1); 
    sem_init(&station_mutex, 0, 1);
    sem_init(&remaining_stations_sem, 0, 0);
    sem_init(&its_sempty, 0, NUM_STATIONS);
    for(int i = 0; i < NUM_STATIONS; i++){
        sem_init(&client_sems[i], 0, 0);
        sem_init(&nurse_sems[i], 0, 0);
    }


    //Develop threads for nurses and clients as the data type pthreads of size NUM_NURSES and NUM_CLIENTS
    pthread_t nurses[NUM_NURSES];
    pthread_t clients[NUM_CLIENTS];


    //Create nurse threads
    /*
    Pseudocode: 
    for i is less than the number of nurses, starting at 0:
    create a thread for nurse (i)
    incremement i
    */
    for(long int i = 0; i < NUM_NURSES; i++){ 
        pthread_create(&nurses[i], NULL, nurse, (void *)i); 
    }

    /*
    Pseudocode:
    for i is less than the number of clients, starting at 0:
    create a thread for client (i)
    increment i
    */
    //Create client threads
    for(long int i = 0; i < NUM_CLIENTS; i++){
        pthread_create(&clients[i], NULL, client, (void*)i);
    }

    //Join nurse threads

    for(long int i = 0; i < NUM_NURSES; i++){
        pthread_join(nurses[i], NULL);
    }
    //Join client threads
    for(long int i = 0; i < NUM_CLIENTS; i++){
        pthread_join(clients[i], NULL);
    }
    pthread_exit(0);
}
