#include <stdio.h>
#include <mpi.h>

/*
 * Computes which processor is entitled to compute or store
 * the lcs lenght at the given 2D vector coordinates.
 * @param a: index of first string character.
 * @param b: index of second string character.
 * @param n: length of first string.
 * @param m: length of second string.
 * @param P: number of processors.
 * 
*/
int proc(int a, int b, int n, int m, int P){

}


int main(int argc, char **argv)
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

    // TODO: se è il processo master parti a calcolare matrice[1][1] e spediscilo a chi di dovere

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
