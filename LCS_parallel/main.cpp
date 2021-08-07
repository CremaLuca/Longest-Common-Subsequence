#include <algorithm>
#include <cmath>
#include <mpi.h>
#include <vector>
#include <stdexcept>
#include <stdio.h>
#include <unordered_map>

using namespace std;

int N, M, P;

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
int cell_diag(pair<int, int> c){
    return c.first + c.second;
}

/**
 * @param c coordinates of a matrix cell
 * @returns The index of the cell w.r.t. the diagonal it belongs to.
 */
int cell_diag_index(pair<int, int> c){
    return min(c.first, N-c.second-1);
}

/**
 * @param c coordinates of a matrix cell
 * @returns The index of the processor responsible for it.
 */
int cell_proc(pair<int, int> c){
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


vector<pair<int, int>> matrix_elements(int i){

    vector<pair<int, int>> v;
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
    char X[] = {'A', 'B', 'C', 'B', 'D', 'A', 'B'};
    char Y[] = {'B', 'D', 'C', 'A', 'B', 'A'};
    // Compute the size of X and Y
    N = sizeof(X)/sizeof(X[0]);
    M = sizeof(Y)/sizeof(Y[0]);

    // Extra: "Currently, MPI_Init takes two arguments that are not necessary, and the extra parameters
    //         are simply left as extra space in case future implementations might need them"
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &P);

    printf("Hello world from process %d of %d\n", rank, P);
    printf("N (size of X) is %d and M (size of Y) is %d\n", N, M);

    // Setup send-request result.

    // Setup local memory
    unordered_map<pair<int, int>, int, hash_pair> local_memory;

    // Compute the list of matrix elements this processor is responsible for.
    vector<pair<int, int>> indices = matrix_elements(rank);

    // Initialize string variable
    string indices_str = "P"+to_string(rank)+" is responsible for indices: ";
    for (int i = 0; i < indices.size(); i++){
        // Concatenate indices to string indices_str
        pair<int, int> c = indices[i];
        indices_str += "("+to_string(c.first) + "," + to_string(c.second) + ") ";
    }
    // Print indices_str to console
    printf("%s\n", indices_str.c_str());

    // Foreach element in the list of indices
    for(size_t index = 0; index < indices.size(); index++){
        pair<int, int> c = indices[index];
        int diagonal = cell_diag(c);
        pair<int, int> up = pair<int, int>(c.first - 1, c.second);
        pair<int, int> left = pair<int, int>(c.first, c.second - 1);
        pair<int, int> up_left = pair<int, int>(c.first - 1, c.second -1);
        int up_value = 0;
        int left_value = 0;
        int up_left_value = 0;

        printf("p%d is working on up (%d, %d), left (%d, %d), up_left (%d, %d)\n", rank, up.first, up.second, left.first, left.second, up_left.first, up_left.second);
        // If the cell is not on the border
        if(up.first >= 0){
            // If the current processor is not responsible for it
            if(cell_proc(up) != rank){ // wait for the value from another processor.
                printf("receiving from p%d\n", cell_proc(up));
                MPI_Recv(&up_value, 1, MPI_INT, cell_proc(up), diagonal-1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                local_memory[up] = up_value;
            }else{  // Grab the value from local memory
                up_value = local_memory[up];
            }
        }
        printf("p%d: up value is %d\n", rank, up_value);

        // If the cell is not on the border
        if(left.second >= 0){
            // If the current processor is not responsible for it
            if(cell_proc(left) != rank){ // wait for the value from another processor.
                printf("receiving from p%d\n", cell_proc(left));
                MPI_Recv(&left_value, 1, MPI_INT, cell_proc(left), diagonal-1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                local_memory[left] = left_value;
            }else{  // Grab the value from local memory
                left_value = local_memory[left];
            }
        }
        printf("p%d: left value is %d\n", rank, left_value);

        up_left_value = 0;
        // We know we have the value of the cell up_left in memory
        if(up_left.first >= 0 && up_left.second >= 0){
            up_left_value = local_memory[up_left];
            if(up_left_value < 0)
                printf("Incorrect value for up_left: (%d, %d) p%d, value=%d\n", up_left.first, up_left.second, rank, up_left_value);

        }

        printf("p%d: up_left value is %d\n", rank, up_left_value);

        // Compute the value of the current cell c
        int c_value = 0;
        if (X[c.first] == Y[c.second]){
            c_value = up_left_value + 1;
        }else{
            c_value = max(up_value, left_value);
        }
        printf("p%d: c_value is %d\n", rank, c_value);
        local_memory[c] = c_value;
        // Send the value to the next processors
        pair<int, int> right = pair<int, int>(c.first, c.second + 1);
        pair<int, int> down = pair<int, int>(c.first + 1, c.second);

        MPI_Request send_req;

        if(down.first < M && cell_proc(down) != rank){
            printf("p%d: sending c_value to p%d\n", rank, cell_proc(down));
            MPI_Isend(&c_value, 1, MPI_INT, cell_proc(down), diagonal, MPI_COMM_WORLD, &send_req);
        }
        else if (right.second < N && cell_proc(right) != rank){
            printf("p%d: sending c_value to p%d\n", rank, cell_proc(right));
            MPI_Isend(&c_value, 1, MPI_INT, cell_proc(right), diagonal, MPI_COMM_WORLD, &send_req);
        }
        // Ignore send request result
        // MPI_Request_free(&send_req);
    }

    // TODO: se sei il master raccogli tutti i dati della matrice e riempila con i valori che ti arrivano

    // TODO: se sei il master ricostruisci il percorso con l'algoritmo sequenziale
    MPI_Finalize();
    return 0;
}
