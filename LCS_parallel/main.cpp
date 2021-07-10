#include <mpi.h>
#include <algorithm>

using namespace std;

int N, M, P;

/**
 * @param d index of the diagonal.
 * @returns Length of the given diagonal.
 */
int diag_length(int d, int M, int N){
    int arr[] = {d+1, M, N, M+N-1-d};
    return *min_element(arr, arr + 3);
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
int cell_diag_index(pair<int, int> c, int N){
    return min(c.first, N-c.second-1);
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
