/*
 * Student Name: Alper Canberk Balcı
 * Student Number: 2017400087
 * Compile Status: Compiling
 * Program Status: Working
 * Notes: The code works perfectly. There may be excessive number of includes.
 */

#include <iostream>
#include <mpi.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <map>
#include <bits/stdc++.h>
#include <vector>

using namespace std;

int main(int argc, char *argv[]) {
    std::ifstream  input(argv[1]); // input file
    int rank; // process id
    int size; // number of processes
    int P, N, A, M, T; // respectively, num of processes, num of instances, num of features, num of iterations, num of top features.
    input >> P >> N >> A >> M >> T; // read the first two lines.
    // they will be sent by broadcasting. But I need to know N and A to initialize an array of two dimensions.
    //cout << P << N << A << M << T;
    double arr[N][A+1]; // array of instances. second dimension holds the features and class variable.
    int info[5];// first two lines of input file.
    MPI_Init(&argc, &argv); // Initialize mpi environment, create processes.

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if(rank == 0){ /*
 * This is the master processor. It provides the slaves with data written from the input file.
 * After slaves have run Relief algorithm in parallel, receive the resulting data (top T features per slave) from them.
 * Eliminate duplicate features. Unite the lists. Print the resulting set to the console.
 */
        int inst_per_proc = N/(P-1);
        for (int i=0; i<N; i++){
            for(int j=0; j<=A;j++){
                input >> arr[i][j];
            }
        }

        info[0] = P;
        info[1] = N;
        info[2] = A;
        info[3] = M;
        info[4] = T;//initialized the info array in master. Now send it to the slaves.

        for(int i=1; i<P; i++){// Send the info array to the slaves.
            MPI_Send(
                    &info[0],
                    5,
                    MPI_INT,
                    i,
                    1,
                    MPI_COMM_WORLD
                    );
        }

        for(int i=0; i<size-1; i++){ // master is sending the lines of information to slave processes.
            MPI_Send(
                     &arr[i*inst_per_proc][0], // buffer. Divide the instances to slaves equally by using the for loop
                     inst_per_proc*(A+1), // count: number of messages. num of instances * number of features per instance.
                     MPI_DOUBLE, //type
                     i+1, //destination: i+1 because loop starts from 0 and slaves starts from 1.
                     2, // tag = 1. This is the second message master sends to slaves.
                     MPI_COMM_WORLD);
        }
        int result[P-1][T];


        for(int i=1; i<=P-1; i++){ // master is receiving the result of relief algorithms run by the slaves.
            MPI_Recv(
                    &result[i-1],
                    T,
                    MPI_INT,
                    i,
                    3,
                    MPI_COMM_WORLD,
                    MPI_STATUS_IGNORE
            );
        }

        std::vector<int> vec;
        for(int i=0; i<P-1;i++){
            for(int j =0; j<T;j++){
                if(std::find(vec.begin(), vec.end(), result[i][j]) == vec.end()){
                    vec.push_back(result[i][j]);
                }
            }
        }
        sort(vec.begin(), vec.end());
        string stri = "Master P0 :";
        for (auto x : vec){
            stri = stri + " " + to_string(x);
        }
        cout << stri << endl;


    }else{ /*
 * This is the slave processor. These are parallel working processors. They are implementing Relief algorithm to
 * eliminate some of the features of instances.
 * 1 - Receive data from master. These are lines of arrays consisting of features and the class variable (0 or 1).
 * 2 - Relief. iterate M times in a for loop. create a weight array, W[A], to find top T features.
 * 3 - Print. print the most valuable features to the console in ascending order with respect to their feature_id.
 * 4 - Send the data to the master. Let the master receive all the data from slaves and work on it.
 */
    //receive the info line: P N A M T
        MPI_Recv(
                &info,
                5,
                MPI_INT,
                0,
                1,
                MPI_COMM_WORLD,
                MPI_STATUS_IGNORE
                );


        int p, n, a, m, t;
        p = info[0];
        n = info[1];
        a = info[2];
        m = info[3];
        t = info[4];
        int inst_per_proc = n/(p-1);

        MPI_Recv( // receive the instances of this process.
                &arr, // data. write into arr. Instances.
                inst_per_proc*(a+1), // count.
                MPI_DOUBLE, // type
                0, // source: master processor's id is zero.
                2, // tag: message id is 1. This message is instances array of two dimensions.
                MPI_COMM_WORLD,
                MPI_STATUS_IGNORE
                );

        // slave got the instances. Now find the max and min of each feature to use it later.

        double min_of_each_feature[a]; //a variable is the number of features.
        double max_of_each_feature[a]; //a variable is the number of features.

        for(int i=0; i<a; i++){
            min_of_each_feature[i] = 99999999999999.0; // float max is 10^38 approximately.
            max_of_each_feature[i] = -999999999999999.0;
        }
        // /////

        // /////
        for(int j = 0; j<a; j++){ // finding the max and min values of each feature.
            for(int i = 0; i<inst_per_proc; i++){ // we don't look at the class variable. so no use of a+1^th value of the instance.
                if(arr[i][j]<min_of_each_feature[j]){
                    min_of_each_feature[j] = arr[i][j];
                }
                if(arr[i][j]>max_of_each_feature[j]){
                    max_of_each_feature[j] = arr[i][j];
                }
            }
        }

        double W[a]; // weight vector for the Relief Algorithm. Initialize it to zeros.
        for(int i=0; i<a; i++){
            W[i] = 0;
        }
        // Relief algorithm
        //double Ri[a+1]; // the current instance in ith iteration. arr[i] basically.
        for(int i = 0; i < m; i++){
            // Ri=arr[i];
            int min_miss_instance = 0;
            int min_hit_instance = 0;
            double distance_hit;
            double distance_miss;
            double distance_hit_min = 99999999999999.0;
            double distance_miss_min = 9999999999999.0;
            for(int j = 0; j < inst_per_proc; j++){
                if(i!=j){
                    distance_hit = 0; // give them 0 because it is a new iteration and a new instance.
                    distance_miss = 0;// we want to find the nearest miss and hit for this feature.

                    if(arr[i][a]==arr[j][a]){// If it is a HIT: if the class variables are the same. 1,1 or 0,0
                        for(int k = 0; k < a; k++){ // a is number of features.
                            distance_hit += std::fabs(arr[i][k] - arr[j][k]);
                        }
                        if(distance_hit < distance_hit_min){ // update the min miss and hit distances and the instances.
                            distance_hit_min = distance_hit;
                            min_hit_instance = j;
                        }
                    }else{ // If it is a MISS: if the class variables are different: 0,1 or 1,0
                        for(int k = 0; k < a; k++){ // a is number of features.
                            distance_miss += std::fabs(arr[i][k] - arr[j][k]);
                        }
                        if(distance_miss < distance_miss_min){
                            distance_miss_min = distance_miss;
                            min_miss_instance = j;
                        }
                    }
                }

            }
            // cout << rank << " rank " << i << "th iteration" << min_hit_instance << " distance hit min "<< distance_hit_min << "xxx" << min_miss_instance << " distance miss min "<< distance_miss_min << endl;
            // we found the nearest miss and nearist hit for arr[i]: ith instance.
            // now we play with the weight vector. It helps us find the most important T features.
            double diff_max_min; // max value of the f-th feature - min value of the f-th feature.
            for(int f = 0; f < a; f++){
                diff_max_min = max_of_each_feature[f] - min_of_each_feature[f];
                W[f] = W[f] - ((std::fabs(arr[i][f] - arr[min_hit_instance][f])/diff_max_min)/m) + ((std::fabs(arr[i][f] - arr[min_miss_instance][f])/diff_max_min)/m);
            }
            // We calculated W weight vector for the ith iteration.
        }// end of m iterations.
        //


        int top_features[t]; // top T features has to be found out.
        double max_i = -9999999999999.0;
        int max_f = 0;

        for(int i=0; i<t; i++){ //iterating for filling top_features.
            for(int j=0;j<a;j++){ // iterating elements of W to find max.
                if(W[j]>max_i){
                    max_i = W[j];
                    max_f = j;

                }
            }// we found the max in W

            W[max_f] = -999999999999999999999999.0; //so that, this feature will not be max again.
            top_features[i] = max_f; // we filled ith value of top_features with the feature
            max_i = -999999999999999999999999999.0; // resetting for other iterations.
        }
        sort(top_features, top_features + t); // sorted the top t features in ascending order.

        string s1 = to_string(int(rank));
        string s2 = "Slave P" + s1 + " :";

        for(int i=0 ; i<t; i++){
            s2 = s2 + " " + to_string(top_features[i]);
        }
        cout << s2 << endl;


        // NOW send top_features to master.
        MPI_Send(
                &top_features[0],
                t,
                MPI_INT,
                0,
                3,
                MPI_COMM_WORLD
        );
    }



    MPI_Finalize();


    return 0;
}