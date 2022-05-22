//Alper Canberk Balcı, 2017400087
//15.02.2021

//Cmpe322 Project2 SyncTicket: A theater ticket selling system
//This is a multithreaded program. There are 3 ticket sellers,
//and many clients that are coming sequentially to the theater
//to buy their tickets for the show.
//if a teller is available, she calls the next client from the queue
//the tellers are searching for the desired seat and if it is empty
//they arrange the seat for the client, but if it is not empty,
//they arrange the minimum numbered empty seat for the client.

//I could not complete the project in time, however I am going to
//finish it tomorrow. I did not have the time to do it.

#include <iostream>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <map>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <bits/stdc++.h>
#include <pthread.h>
#include <time.h>
#include <sstream>
#include <ctime>

using namespace std;
int numOfClients;
int numOfSeats;
int tellerName;
string theater;
//mutex lock for client
pthread_mutex_t clientLock;
//teller lock
pthread_mutex_t tellerLock[3];
sem_t teller_sem;

// output stream
ofstream fout;
string lines; //input lines
vector<int> seats; //seat vector for searching available seats
//client information global. these will be looked for by the teller.
string client_name;
int arrival_time;
int service_time;
int seat_num;

//client func
void *client(void *param);

//teller func
void *teller(void *param);


int main(int argc, char *argv[]) {
    ifstream fin(argv[1]); // input file
    fout(argv[2]); // output file
    fout << "Welcome to the Sync-Ticket!" << endl;

    fin >> theater >> numOfClients; // read first two lines

    if(theater == "OdaTiyatrosu"){
        numOfSeats = 60;
    }else if(theater == "UskudarStudyoSahne"){
        numOfSeats = 80;
    }else if(theater == "KucukSahne"){
        numOfSeats = 200;
    }
    //initialize mutexes
    pthread_mutex_init(&clientLock, NULL);
    for(int i=0; i<3;i++){
        pthread_mutex_init(&tellerLock, NULL);
    }
    sem_init(&teller_sem, 0, 3);

    //create the seats
    for(int i=0; i<numOfSeats;i++){
        seats.push_back(-1);
        //-1 means it is available.
    }

    lines[numOfClients];
    for(int i = 0; i<numOfClients;i++){
        fin >> lines[i];
    }
    fin.close();// reader has read everything so we close it.

    pthread_t tellerThreads[3]; // teller A,B,C
    pthread_attr_t tellerThreadAttr;
    pthread_attr_init(&tellerThreadAttr);
    //creating teller threads
    for(int i = 0; i < 3; i++){

        tellerName = i;
        pthread_create(&tellerThreads[i], &tellerThreadAttr,teller,&tellerName);
    }

    pthread_t clientThreads[numOfClients]; //create the clients
    pthread_attr_t clientThreadAttr;
    pthread_attr_init(&clientThreadAttr);
    //create the clients
    for(int i = 0; i < numOfClients; i++){
        pthread_create(&clientThreads[i], &clientThreadAttr,client,&lines[i]);

    }
    //join the threads
    for(int i =0; i<3;i++){
        pthread_join(tellerThreads[i];NULL);
    }
    //to wait the client queue to be empty
    for(int i =0; i<numOfClients;i++){
        pthread_join(clientThreads[i];NULL);
    }
    //destroy mutexes
    pthread_mutex_destroy(&clientLock);
    pthread_mutex_destroy(&tellerLock);
    sem_destroy(&teller_sem);
    return 0;
}

//teller function. It prints its name. If he is available, he does the
//searching for the desired seat, and arrange it.
void *teller(void *param){
    string name; // name of the teller A, B, or C
    int namex = *(int *)param;
    if(namex==0){
        name="A";
    }else if(namex==1){
        name="B";
    }else if(namex==2){
        name="C";
    }
    fout << "Teller " << name << " has arrived." <<endl;
    //usleep(1000*50); //sleep for 50 ms for sync.
    //critical section enter
    sem_wait(&teller_sem); //3 semaphores for 3 tellers
    pthread_mutex_lock(&tellerLock[namex]);
    //arrange the desired seat.

    // I have no time it is 13:50 o'clock.
    //end critical section
    pthread_mutex_unlock(&tellerLock[namex]);
    sem_post(&teller_sem);
}

void *client(void *param){
    string line = *(string *)param;
    stringstream ss(line);
    vector<string> vec;
    while(ss.good()){
        string substr;
        getline(ss,substr,',');
        vec.push_back(substr);
    }
    string this_client_name = vec[0];
    int this_arrival_time = stoi(vec[1]);
    int this_service_time = stoi(vec[2]);
    int this_seat_num = stoi(vec[3]);



/*
    client_name = vec[0];
    arrival_time = stoi(vec[1]);
    service_time = stoi(vec[2]);
    seat_num = stoi(vec[3]);

 */
    //sleep until arrival time
    usleep(1000*arrival_time);
    //client arrived do his work.
    //search for available tellers. priority is A, then B, then C.
    //start critical section
    //
    // use global variables to talk to teller
    sem_wait(&teller_sem); //3 semaphores for 3 tellers
    pthread_mutex_lock(&clientLock);
    client_name = this_client_name ;
    arrival_time = this_arrival_time;
    service_time = this_service_time;
    seat_num = this_seat_num;

    //end critical section
    pthread_mutex_unlock(&clientLock);
    sem_post(&teller_sem);
    //sleep while service time
    usleep(1000*service_time);
}