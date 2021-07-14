#include <algorithm>
#include <cmath>
#include <mpi/mpi.h>
#include <vector>

using namespace std;

int N, M, P;

/**
 * @param d index of the diagonal.
 * @returns Length of the given diagonal.
 */
int diag_length(int d){
    int arr[] = {d+1, M, N, M+N-1-d};
    return *min_element(arr, arr + 3);
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


int cell_proc(pair<int, int> c){
    int d = cell_diag(c);
    int len = diag_length(d);
    int pos = cell_diag_index(c);
    int p = min(len, P);
    int ceil_size = ceil((float)len/p);
    int floor_size = floor((float)len/p);
    int rem = len % p;
    if (pos < ceil_size * rem)
        return floor((float) pos / ceil_size);
    else
        return floor((float)(pos - rem) / floor_size);
}


vector<pair<int, int>> matrix_elements(int i){

    vector<pair<int, int>> v;

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


int main()
{
    int rank, P;
    char X[] = {'A', 'B', 'C', 'B', 'D', 'A', 'B'};
    char Y[] = {'B', 'D', 'C', 'A', 'B', 'A'};
    // Extra: "Currently, MPI_Init takes two arguments that are not necessary, and the extra parameters
    //         are simply left as extra space in case future implementations might need them"

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &P);

    printf("Hello world from process %d of %d\n", rank, P);

    // TODO: inizializza la matrice con la prima riga e colonna a 0

    // TODO: calcola la lista di coppie di indici della matrice che questo processo deve calcolare

    // TODO: per ogni coppia di indici assegnata a questo processo
        // TODO: se i valori necessari per calcolare la cella corrente non sono già nella matrice
            // TODO: aspetta di riceverli da chi di dovere in blocking RECV

        // TODO: calcola la cella della coppia di indici corrente

        // TODO: manda in non-blocking send il valore al processo che ne ha bisogno (se diverso)

    // TODO: se non sei master manda tutta la matrice al processo master

    // TODO: se sei il master raccogli tutti i dati della matrice e riempila con i valori che ti arrivano

    // TODO: se sei il master ricostruisci il percorso con l'algoritmo sequenziale

    MPI_Finalize();
    return 0;
}
