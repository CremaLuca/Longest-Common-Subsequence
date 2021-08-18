#include <mpi.h>
#include <stdexcept>
#include <unordered_map>
#include <fstream>
#include <chrono>

#include "utils.h"

using namespace std;

int N, M, P;

//M ROWS, N COLUMNS, P PROCESSORS

int main(int argc, char ** argv)
{
    //Read the input file containing the sequences

    if(argc < 2){
        printf("Wrong usage: pass the path to the input file!\n");
        return 1;
    }

    char * path = argv[1];

    ifstream infile;
    infile.open(path);
    if(!infile.is_open()){
        printf("Make sure the input file exists!\n");
        return 1;
    }

    string X;
    string Y;

    std::getline(infile, X);
    std::getline(infile, Y);


    if(infile.fail()){
        printf("Make sure the input file consists of two lines!\n");
        return 1;
    }

    infile.close();

    M = X.size();
    N = Y.size();

    //Make sure M <= N
    //WLOG IT'S ASSUMED M <= N IN THE FOLLOWING

    if(M > N) {
        swap(M, N);
        swap(X, Y);
    }

    int rank;

#ifndef NDEBUG
    printf("M (size of X) is %d and N (size of Y) is %d\n", M, N);
#endif

    // Currently, MPI_Init takes two arguments that are not necessary, and the extra parameters
    // are simply left as extra space in case future implementations might need them
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &P);

    chrono::steady_clock::time_point begin_time = chrono::steady_clock::now();

#ifndef NDEBUG
    printf("Hello world from process %d of %d\n", rank, P);
#endif
    
    // Setup local memory
    unordered_map<cell, int, hash_pair> local_memory;

    // Compute the list of matrix elements this processor is responsible for
    vector<cell> indices = matrix_elements(rank);

#ifndef NDEBUG
    // Print cells assigned to this processor
    string indices_str = "P" + to_string(rank) + " is responsible for indices: ";
    for (size_t i = 0; i < indices.size(); i++){
        // Concatenate indices to string indices_str
        cell c = indices[i];
        indices_str += "("+to_string(c.first) + "," + to_string(c.second) + ") ";
    }

    // Print indices_str to console
    printf("%s\n", indices_str.c_str());

#endif

    //********************************
    //BEGIN PROCESSING LOOP
    //********************************

    // Foreach element in the list of indices
    for(size_t index = 0; index < indices.size(); index++){
        cell current_cell = indices[index];
        int diagonal = cell_diag(current_cell);
        cell up = cell(current_cell.first - 1, current_cell.second);
        cell left = cell(current_cell.first, current_cell.second - 1);
        cell up_left = cell(current_cell.first - 1, current_cell.second -1);
        int up_value = 0;
        int left_value = 0;
        int up_left_value = 0;

        // If the cell is not on the border
        if(up.first >= 0){
            // If the current processor is not responsible for it
            int proc_up = cell_proc(up);
            if(proc_up != rank){ // wait for the value from another processor.
                MPI_Recv(&up_value, 1, MPI_INT, proc_up, diagonal-1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                local_memory[up] = up_value;
#ifndef NDEBUG
                printf("p%d: receiving up (%d, %d) from p%d\n", rank, up.first, up.second, proc_up);
#endif
            }else{  // Grab the value from local memory
                up_value = local_memory[up];
            }
        }

        // If the cell is not on the border
        if(left.second >= 0){
            // If the current processor is not responsible for it
            int proc_left = cell_proc(left);
            if(proc_left != rank){ // wait for the value from another processor.
                MPI_Recv(&left_value, 1, MPI_INT, proc_left, diagonal-1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                local_memory[left] = left_value;
#ifndef NDEBUG
                printf("p%d: receiving left (%d, %d) from p%d\n", rank, left.first, left.second, proc_left);
#endif
            }else{  // Grab the value from local memory
                left_value = local_memory[left];
            }
        }

        up_left_value = 0;
        // We know we have the value of the cell up_left in memory
        if(up_left.first >= 0 && up_left.second >= 0){
            up_left_value = local_memory[up_left];
        }

        // Compute the value of the current cell c
        int c_value = 0;
        if (X[current_cell.first] == Y[current_cell.second]){
            c_value = up_left_value + 1;
        }else{
            c_value = max(up_value, left_value);
        }

#ifndef NDEBUG
        printf("p%d is working on chars %c, %c at (%d, %d) = %d, with up (%d, %d) = %d,"
               " left (%d, %d) = %d,"
               " up_left (%d, %d) = %d\n",
               rank, X[current_cell.first], Y[current_cell.second], current_cell.first, current_cell.second, c_value, up.first, up.second,
                up_value, left.first, left.second, left_value, up_left.first, up_left.second, up_left_value);
#endif

        // Store the value in local memory
        local_memory[current_cell] = c_value;

        // Send the value to the next processors
        cell right = cell(current_cell.first, current_cell.second + 1);
        cell down = cell(current_cell.first + 1, current_cell.second);

        MPI_Request send_req;

        int rec_proc;
        if(down.first < M && (rec_proc = cell_proc(down)) != rank){
#ifndef NDEBUG
            printf("p%d: sending c_value to p%d\n", rank, rec_proc);
#endif
            MPI_Isend(&c_value, 1, MPI_INT, rec_proc, diagonal, MPI_COMM_WORLD, &send_req);
        }
        else if (right.second < N && (rec_proc = cell_proc(right)) != rank){
#ifndef NDEBUG
            printf("p%d: sending c_value to p%d\n", rank, rec_proc);
#endif
            MPI_Isend(&c_value, 1, MPI_INT, rec_proc, diagonal, MPI_COMM_WORLD, &send_req);
        }
        // Ignore send request result
        //MPI_Request_free(&send_req);
    }
    // Wait for all processes to finish before starting to reconstruct the LCS
    MPI_Barrier(MPI_COMM_WORLD);

#ifndef NDEBUG
    if(rank == 0){
        printf("All processes are done for the first part and the LCS value is %d\n", local_memory[cell(M-1, N-1)]);
    }
#endif

    //********************************
    //END PROCESSING LOOP
    //********************************


    //********************************
    //BEGIN LCS RECONSTRUCTION LOOP
    //********************************


    string lcs;
    int i,j;

    bool begin_reconstruction = false;
    if(rank == 0) {
        begin_reconstruction = true;
        i = M - 1;
        j = N - 1;
#ifndef NDEBUG
        printf("p0: starting the LCS reconstruction\n");
#endif
    }

    while(true){

        if(begin_reconstruction){
            // Start the LCS reconstruction

            while(i >= -1 && j >= -1){

                if(i == -1 || j == -1){ //We passed the border of the lcs matrix, so the computation halts
                    // Send stop signal to all other procesors
                    for(int p = 0; p < P; p++)
                        if(p != rank){
                            MPI_Send(nullptr, 0, MPI_CHAR, p, STOP_TAG, MPI_COMM_WORLD);
                        }
                    chrono::steady_clock::time_point end_time = chrono::steady_clock::now();
                    long int int_us = chrono::duration_cast<chrono::microseconds>(end_time - begin_time).count();
                    //Save output to file if specified, then print the lcs and goto done
                    if(argc >= 3){
                        path = argv[2];
                        ofstream oufile;
                        oufile.open(path, ios_base::trunc | ios_base::out);
                        if(oufile.fail()){
                            printf("Cannot save output to file!\n");
                            return 1;
                        }
                        oufile << lcs << endl << int_us << " [µs]";
                        oufile.close();
                    }

                    printf("Parallel output by p%d: %s\nTook %ld us\n", rank, lcs.c_str(), int_us);
                    goto done;
                }

                cell current = cell(i, j);
                cell up = cell(i-1, j);
                cell left = cell(i, j-1);
                cell up_left = cell(i-1, j-1);
                int current_value = local_memory[current];
                int up_value = local_memory[up];
#ifndef NDEBUG
                printf("p%d: c_value: %d, up_value: %d, i: %d, j: %d, X[i]: %c, Y[j]: %c\n", rank, current_value, up_value, i, j, X[i], Y[j]);
#endif
                if(X[i] == Y[j]){
#ifndef NDEBUG
                    printf("p%d: appending char %c\n", rank, X[i]);
#endif
                    lcs = X[i] + lcs;
                    i--; j--;
                }else if (current_value == up_value){
                    i--;
                }else { //if current_value == left_value
                    j--;
                }

                cell next = cell(i, j);
                int next_proc = cell_proc(next);
                // Send the coordinates to the next processor so that it can continue computing the lcs
                if(next_proc != rank && next_proc != -1){

                    //if == -1, then we passed the border, and back at the beginning of the loop termination is done
#ifndef NDEBUG
                    printf("p%d: sending %s and (%d, %d) to p%d\n", rank, lcs.c_str(), i, j, next_proc);
#endif
                    MPI_Send(&lcs[0], lcs.size(), MPI_CHAR, next_proc, COMPUTE_LCS_TAG, MPI_COMM_WORLD);
                    MPI_Send(std::begin({i, j}), 2, MPI_INT, next_proc, COMPUTE_LCS_TAG, MPI_COMM_WORLD);
                    begin_reconstruction = false;
                    break;
                }
            }
        }

        MPI_Status status;
        // Find out the length and the tag of the message without emptying the buffer
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        // If we received a stop signal, break
        if(status.MPI_TAG == STOP_TAG){
            goto done;
        } else if (status.MPI_TAG == COMPUTE_LCS_TAG){
            //Receive the lcs
            int len;
            MPI_Get_count(&status, MPI_CHAR, &len);
            char* buffer = new char[len+1];
            buffer[len] = '\0';
            MPI_Recv(buffer, len, MPI_CHAR, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);
            lcs = buffer;

            //Next receive the position

            int* lcs_coords = new int[2];
            MPI_Recv(lcs_coords, 2, MPI_INT, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);
            i = lcs_coords[0];
            j = lcs_coords[1];
            begin_reconstruction = true;

#ifndef NDEBUG
            printf("p%d: computing lcs starting from (%d, %d)\n", rank, i, j);
#endif
        }
    }

    //********************************
    //END LCS RECONSTRUCTION LOOP
    //********************************

done:
    MPI_Finalize();
    return 0;
}
