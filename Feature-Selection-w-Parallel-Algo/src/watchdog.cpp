#include <iostream>
#include <string>
#include <unistd.h>
#include <csignal>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <map>
#include <stdlib.h>
#include <cstring>

using namespace std;
map<string,int> sigMap;
string pout_path;
string wout_path;
ofstream pout;
ofstream wout;
map<pid_t, int> processMap; // key: PID, value: P# (process number)
int unnamedPipe;
struct timespec delta = {0 /*secs*/, 800000000 /*nanosecs*/}; //0.8 sec can be changed.

void signalHandler(int signum) { // WOUT YAP SUNLARI DUZELT

    if(signum==15){
        wout.open(wout_path, ios::app);
        wout << "Watchdog is terminating gracefully" << endl;
        wout.close();
        close(unnamedPipe);
        nanosleep(&delta, &delta);  // Deal with writing delays
        exit(signum);
    }

}

int main(int argc, char *argv[]) {

    signal(SIGTERM, signalHandler); //15 //when instructions finishes, watchdog has to be killed
    pid_t childpid = 0;
    int n; // number of processes
    n = stoi(argv[1]);
    pout_path = argv[2];
    wout_path = argv[3];
    pid_t pidList[n+1]; // Keep PID of watchdog at 0, PID of P1 at 1, ...
    pidList[0] = getpid();
    string s = "";
    char arr[30];


    char * myfifo = (char*) "/tmp/myfifo";
    mkfifo(myfifo, 0666);
    char temp[30];
    // int unnamedPipe; in global variables.
    unnamedPipe = open(myfifo,O_WRONLY);

    string wd_id = to_string((int)getpid());
    string p0 = "P0 " + wd_id;
    strcpy(temp, p0.c_str());
    write(unnamedPipe, temp, strlen(temp)+1); // send watchdog "P0 <pid>" through the pipe

    string pr_id = "";
    string pr_num = "";
    string pipe_msg = "";
    char temp_pipe_msg[30];
    char proc_out_path[30];
    for(int i = 1; i <= n; i++){ // Create all the processes

        childpid = fork();
        pr_num = "P" + to_string(i);
        pr_id = to_string((int)childpid);
        pipe_msg = pr_num + " " + pr_id;
        strcpy(temp_pipe_msg, pipe_msg.c_str());
        // exec if else li bi şeydi galiba
        if(childpid == -1){
            perror("fork failed");
            return 1;
        }
        else if(childpid == 0){

            s = to_string(i);
            strcpy(arr, s.c_str());
            strcpy(proc_out_path, pout_path.c_str());
            execl("./process", "./process", arr, proc_out_path, NULL); // created a process, i parameter is process number
            //exec. execle beraber processNum u parametre olarak verebilirsin
            nanosleep(&delta, &delta);  // Deal with writing delays

        }else{
            pidList[i] = childpid;
            processMap[childpid] = i;//pid_t olmazsa type cast it to long


            write(unnamedPipe, temp_pipe_msg,strlen(temp_pipe_msg)+1); // send p# pid thru fifo to executor.
            wout.open(wout_path, ios::app);
            wout << "P" << i << " is started and it has a pid of "<< childpid << endl;
            wout.close();
            nanosleep(&delta, &delta);  // Deal with writing delays

        }
        // send P# and PID thru fifo
    }


    pid_t process_id_died;

    // watchdog sleeps until a process is killed.
    while((process_id_died = wait(NULL)) > 0){ //killed process' id is returned.

        int process_num_died = processMap[process_id_died];

        if(process_id_died == pidList[1]){
            //If P1 died, kill all other processes. Restart them. Send P#, PID tuple thru fifo
            wout.open(wout_path, ios::app);
            wout << "P1 is killed, all processes must be killed"<< endl;
            wout << "Restarting all the processes"<< endl;
            wout.close();

            pid_t temp_childpid;
            processMap.erase(process_id_died); // erase P1 from the map
            pid_t pid_i; // for loop variable
            // killing all other processes
            for(int i = 2; i<=n; i++){
                pid_i = pidList[i];
                kill(pid_i, SIGTERM); // terminate the processes in increasing order
                anosleep(&delta, &delta);  // Deal with writing delays
                // sleep
                processMap.erase(pid_i); // erase the killed process from map

            }
            // restarting process
            for(int i = 1; i<=n; i++){

                temp_childpid = fork(); // create the ith process

                pr_num = "P" + to_string(i);
                pr_id = to_string((int)temp_childpid);
                pipe_msg = pr_num + " " + pr_id;
                strcpy(temp_pipe_msg, pipe_msg.c_str());


                if(temp_childpid == -1){
                    perror("fork failed");
                    return 1;
                }
                if(temp_childpid == 0){
                    s = to_string(i);
                    strcpy(arr, s.c_str());
                    strcpy(proc_out_path, pout_path.c_str());
                    execl("./process", "./process", arr, proc_out_path, NULL); // created a process via execl, i parameter is process number

                }else{
                    processMap[temp_childpid] = i; // add the pid to map
                    pidList[i] = temp_childpid;

                    write(unnamedPipe, temp_pipe_msg,strlen(temp_pipe_msg)+1); // send p# pid thru fifo to executor.
                    wout.open(wout_path, ios::app);
                    wout << "P" << i << " is started and it has a pid of "<< temp_childpid << endl;
                    wout.close();
                    nanosleep(&delta, &delta);  // Deal with writing delays

                }
                //send P# and PID thru fifo
            }
        }else{
            // executor already killed it. Watchdog should restart the killed process.
            pid_t temp_childpid;
            processMap.erase(process_id_died); // erase the killed process from the map

            wout.open(wout_path, ios::app);
            wout << "P" << process_num_died << " is killed" << endl;
            wout << "Restarting P" << process_num_died << endl;
            wout.close();

            temp_childpid = fork(); // restart the killed process

            pr_num = "P" + to_string(process_num_died);
            pr_id = to_string((int)temp_childpid);
            pipe_msg = pr_num + " " + pr_id;
            strcpy(temp_pipe_msg, pipe_msg.c_str());
            //exec
            if(temp_childpid == -1){
                perror("fork failed");
                return 1;
            }
            if(temp_childpid == 0){ // create the process
                s = to_string(process_num_died);
                strcpy(arr, s.c_str());
                strcpy(proc_out_path, pout_path.c_str());
                execl("./process", "./process", arr, proc_out_path, NULL); // created a process via execl, i parameter is process number

            }else{ // parent
                processMap[temp_childpid] = process_num_died; // add it to the map again
                pidList[process_num_died] = temp_childpid; // fix the pid of the process
                write(unnamedPipe, temp_pipe_msg,strlen(temp_pipe_msg)+1); // send p# pid thru fifo to executor.
                nosleep(&delta, &delta);  // Deal with writing delays
                wout.open(wout_path, ios::app);
                wout << "P" << process_num_died << " is started and it has a pid of "<< temp_childpid << endl;
                wout.close();
                nanosleep(&delta, &delta);  // Deal with writing delays
            }
            //sleep nanosleep(&delta, &delta);  // Deal with writing delays
            //send P# and PID thru fifo
            //sleep

        }
    }

    return 0;
}
