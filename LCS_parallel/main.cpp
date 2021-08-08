#include <algorithm>
#include <cmath>
#include <mpi.h>
#include <vector>
#include <stdexcept>
#include <stdio.h>
#include <unordered_map>
#include <iostream>

using namespace std;


typedef pair<int, int> cell;

const int COMPUTE_LCS_TAG = 1;
const int STORE_LCS_TAG = 2;
const int STOP_TAG = 3;

int N, M, P;

//M ROWS, N COLUMNS, P PROCESSORS

/**
 * @param d index of the diagonal.
 * @returns Length of the given diagonal.
 */
int diag_length(int d){
    int arr[] = {d+1, M, N, M+N-1-d};
    return *min_element(arr, arr + 4);
}

/**
 * @param d index of the diagonal.
 * @param i index of the processor
 * @returns a pair (s, e) indicating the starting/ending cell on
 * the diagonal d assigned to processor i. index e is exclusive,
 * while index s in inclusive.
 */
pair<int, int> diag_start_end(int d, int i){
    int len = diag_length(d);
    int p = min(P, len);
    int ceil_size = ceil((float)len/p);
    int floor_size = floor((float)len/p);
    int rem = len % p;
    int start, end;

    //printf("len: %d, p: %d, ceil: %d, floor: %d, rem: %d\n", len, p, ceil_size, floor_size, rem);

    if (i < rem){
        start = i * ceil_size;
        end = start + ceil_size;
    }
    else{
        start = rem * ceil_size + (i-rem) * floor_size;
        end = start + floor_size;
    }
    return pair<int, int>(start, end);
}


/**
 * @param c coordinates of a matrix cell
 * @returns Index of the diagonal where the cell belongs
 */
int cell_diag(cell c){
    return c.first + c.second;
}

/**
 * @param c coordinates of a matrix cell
 * @returns The index of the cell w.r.t. the diagonal it belongs to.
 */
int cell_diag_index(cell c){
    return min(c.first, N-c.second-1);
}

/**
 * @param c coordinates of a matrix cell
 * @returns The index of the processor responsible for it.
 */
int cell_proc(cell c){
    if(c.first < 0 || c.second < 0){
        return -1;
    }
    int d = cell_diag(c);
    int len = diag_length(d);
    int pos = cell_diag_index(c);
    int p = min(len, P);
    int ceil_size = ceil(((float)len)/p);
    int floor_size = floor(((float)len)/p);
    int rem = len % p;

    //printf("cell (%d, %d), diag %d, len %d, pos %d, ceil %d, floor %d, rem %d\n", c.first, c.second, d, len, pos, ceil_size, floor_size, rem);

    if (pos < ceil_size * rem)
        return floor(((float) pos) / ceil_size);
    else
        return floor(((float)(pos - rem)) / floor_size);
}


vector<cell> matrix_elements(int i){

    vector<cell> v;
    // If there are more processor than elements, ignore processor
    if(i >= min(M, N))
        return v;

    for(int d = i; d < N+M-1-i; d++){
        pair<int, int> indices = diag_start_end(d, i);
        for(int e = indices.first; e < indices.second; e++){
            int x = max(0, d-N+1) + e;
            int y = min(d, N-1) - e;
            v.push_back(pair<int, int>(x, y));
        }
    }
    return v;
}

// A hash function used to hash a pair of any kind
struct hash_pair {
    template <class T1, class T2>
    size_t operator()(const pair<T1, T2>& p) const
    {
        auto hash1 = hash<T1>{}(p.first);
        auto hash2 = hash<T2>{}(p.second);
        return hash1 ^ hash2;
    }
};

int main()
{
    int rank;

    char X[] = {'B', 'D', 'C', 'A', 'B', 'A'};
    char Y[] = {'A', 'B', 'C', 'B', 'D', 'A', 'B'};
    // Compute the size of X and Y

    M = sizeof(X)/sizeof(X[0]);
    N = sizeof(Y)/sizeof(Y[0]);

    printf("N (size of Y) is %d and M (size of X) is %d\n", N, M);

    // Extra: "Currently, MPI_Init takes two arguments that are not necessary, and the extra parameters
    //         are simply left as extra space in case future implementations might need them"
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &P);

    printf("Hello world from process %d of %d\n", rank, P);
    
    // Setup send-request result.

    // Setup local memory
    unordered_map<cell, int, hash_pair> local_memory;

    // Compute the list of matrix elements this processor is responsible for.
    vector<cell> indices = matrix_elements(rank);

    // Initialize string variable
    string indices_str = "P" + to_string(rank) + " is responsible for indices: ";
    for (size_t i = 0; i < indices.size(); i++){
        // Concatenate indices to string indices_str
        cell c = indices[i];
        indices_str += "("+to_string(c.first) + "," + to_string(c.second) + ") ";
    }

    // Print indices_str to console
    printf("%s\n", indices_str.c_str());


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
                printf("p%d: receiving up (%d, %d) from p%d\n", rank, up.first, up.second, proc_up);
                MPI_Recv(&up_value, 1, MPI_INT, proc_up, diagonal-1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                local_memory[up] = up_value;
            }else{  // Grab the value from local memory
                up_value = local_memory[up];
            }
        }

        // If the cell is not on the border
        if(left.second >= 0){
            // If the current processor is not responsible for it
            int proc_left = cell_proc(left);
            if(proc_left != rank){ // wait for the value from another processor.
                printf("p%d: receiving left (%d, %d) from p%d\n", rank, left.first, left.second, proc_left);
                MPI_Recv(&left_value, 1, MPI_INT, proc_left, diagonal-1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                local_memory[left] = left_value;
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

        printf("p%d is working on %c %c (%d, %d) = %d, with up (%d, %d): %d,"
               " left (%d, %d): %d,"
               "up_left (%d, %d): %d\n",
               rank, X[current_cell.first], Y[current_cell.second], current_cell.first, current_cell.second, c_value, up.first, up.second,
               up_value, left.first, left.second, left_value, up_left.first, up_left.second, up_left_value);

        // Store the value in local memory
        local_memory[current_cell] = c_value;
        // Send the value to the next processors
        cell right = cell(current_cell.first, current_cell.second + 1);
        cell down = cell(current_cell.first + 1, current_cell.second);

        MPI_Request send_req;

        int rec_proc;
        if(down.first < M && (rec_proc = cell_proc(down)) != rank){
            printf("p%d: sending c_value to p%d\n", rank, rec_proc);
            MPI_Isend(&c_value, 1, MPI_INT, rec_proc, diagonal, MPI_COMM_WORLD, &send_req);
        }
        else if (right.second < N && (rec_proc = cell_proc(right)) != rank){
            printf("p%d: sending c_value to p%d\n", rank, rec_proc);
            MPI_Isend(&c_value, 1, MPI_INT, rec_proc, diagonal, MPI_COMM_WORLD, &send_req);
        }
        // Ignore send request result
        // MPI_Request_free(&send_req);
    }
    // Wait for all processes to finish before starting to reconstruct the LCS
    MPI_Barrier(MPI_COMM_WORLD);
    if(rank == 0){
        printf("All processes are done for the first part and the LCS value is %d.\n", local_memory[cell(M-1, N-1)]);
    }

    //********************************
    //END PROCESSING LOOP
    //********************************


    //********************************
    //BEGIN LCS RECONSTRUCTION LOOP
    //********************************


    MPI_Request send_req;
    string lcs;
    int i,j;

    bool begin_reconstruction = false;
    if(rank == 0) {
        begin_reconstruction = true;
        i = M - 1;
        j = N - 1;
        printf("p0: starting the LCS reconstruction\n");
    }

    while(true){

        if(begin_reconstruction){
            // Start the LCS reconstruction

            while(i >= -1 && j >= -1){

                if(i == -1 || j == -1){ //LCS DONE
                    // Send stop signal to all other procesors
                    for(int i = 0; i < P; i++)
                        if(i != rank)
                            MPI_Isend(nullptr, 0, MPI_CHAR, i, STOP_TAG, MPI_COMM_WORLD, &send_req);

                    //TODO PRINT THE LCS AND QUIT
                    goto done;
                }

                cell current = cell(i, j);
                cell up = cell(i-1, j);
                cell left = cell(i, j-1);
                cell up_left = cell(i-1, j-1);
                int current_value = local_memory[current];
                int up_value = local_memory[up];
                printf("p%d: cv: %d, up: %d, i: %d, j: %d, X[i]: %c, Y[j]: %c\n", rank, current_value, up_value, i, j, X[i], Y[j]);
                if(X[i] == Y[j]){
                    printf("p%d: appending char %c\n", rank, X[i]);
                    lcs = X[i] + lcs;
                    i--; j--;
                }else if (current_value == up_value){
                    i--;
                }else { //if current_value == left_value
                    j--;
                }

                cell next = cell(i, j);
                int next_proc = cell_proc(next);
                // Send the coordinates to the next processor so that it can continue and the lcs to p0
                if(next_proc != rank){
                    if(rank != 0){
                        printf("p%d: sending %s to p0\n", rank, lcs.c_str());
                        MPI_Isend(lcs.c_str(), lcs.size(), MPI_CHAR, 0, STORE_LCS_TAG, MPI_COMM_WORLD, &send_req);
                    }else{
                        printf("p0: computed new lcs %s\n", lcs.c_str());
                    }
                    if(next_proc != -1){
                        printf("p%d: sending coords (%d, %d) to p%d\n", rank, i, j, next_proc);
                        MPI_Isend(std::begin({i, j}), 2, MPI_INT, next_proc, COMPUTE_LCS_TAG, MPI_COMM_WORLD, &send_req);
                        begin_reconstruction = false;
                        break;
                    }
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
            // Read the message
            int* lcs_coords = new int[2];
            MPI_Recv(lcs_coords, 2, MPI_INT, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);
            i = lcs_coords[0];
            j = lcs_coords[1];
            if(rank != 0)
                lcs = "";
            begin_reconstruction = true;
            printf("p%d: computing lcs starting from (%d, %d)\n", rank, i, j);
        } else if (status.MPI_TAG == STORE_LCS_TAG){ // Only processor 0 will receive this message tag
            // Read the lcs
            int lcs_length;
            MPI_Get_count(&status, MPI_CHAR, &lcs_length);
            char* lcs_buffer = new char[lcs_length+1];
            lcs_buffer[lcs_length] = '\0';
            MPI_Recv(lcs_buffer, lcs_length, MPI_CHAR, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);
            printf("p0: received %d character of lcs %s\n", lcs_length, lcs_buffer);
            lcs = lcs_buffer + lcs;
        }
    }



    //********************************
    //END LCS RECONSTRUCTION LOOP
    //********************************

done:
    MPI_Barrier(MPI_COMM_WORLD);
    if(rank == 0){
        printf("p0: reconstructed lcs string %s\n", lcs.c_str());
    }
    MPI_Finalize();
    return 0;
}
