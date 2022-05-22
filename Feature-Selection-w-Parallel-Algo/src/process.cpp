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

using namespace std;
map<string,int> sigMap;
int processNum;
string pout_path;
struct timespec delta = {0 /*secs*/, 800000000 /*nanosecs*/}; //0.3 sec
ofstream fout;

void signalHandler(int signum) {

    fout.open(pout_path, ios::app);
    if(signum!=15){
        fout << "P" << processNum << " received signal " << signum << endl;
        nanosleep(&delta, &delta);  // Deal with writing delays
    }
    else{
        fout << "P" << processNum << " received signal 15, terminating gracefully" << endl;
        nanosleep(&delta, &delta);  // Deal with writing delays
        exit(signum);
    }
    fout.close();

}

int main(int argc, char *argv[]) {
    signal(SIGBUS, signalHandler);  //7
    signal(SIGHUP, signalHandler);  //1
    signal(SIGINT, signalHandler);  //2
    signal(SIGILL, signalHandler);  //4
    signal(SIGTRAP, signalHandler); //5
    signal(SIGFPE, signalHandler);  //8
    signal(SIGSEGV, signalHandler); //11
    signal(SIGTERM, signalHandler); //15
    signal(SIGXCPU, signalHandler); //24

    processNum = stoi(argv[1]); // read from the command line argument for process number value
    pout_path = argv[2];
    fout.open(pout_path, ios::app); // open in append mode
    fout << "P" << processNum << " is waiting for a signal" << endl;
    fout.close();
    nanosleep(&delta, &delta);  // Deal with writing delays

    return 0;
}
