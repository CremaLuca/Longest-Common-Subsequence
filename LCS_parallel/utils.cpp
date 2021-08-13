#include "utils.h"

#include <stdio.h>

extern int N, M, P;


/**
 * @param d Index of the diagonal
 * @returns Length of the given diagonal
 */
int diag_length(int d){
    int arr[] = {d+1, M, M+N-1-d};
    return *std::min_element(arr, arr + 3);
}


/**
 * @param d Index of the diagonal
 * @param i Index of the processor
 * @returns A pair (s, e) indicating the starting/ending cell on
 * the diagonal d assigned to processor i. Index e is exclusive,
 * while index s in inclusive
 */
std::pair<int, int> diag_start_end(int d, int i){
    int len = diag_length(d);
    int p = std::min(P, len);
    int ceil_size = ceil((float)len/p);
    int floor_size = floor((float)len/p);
    int rem = len % p;
    int start, end;

#ifndef NDEBUG
    printf("len: %d, p: %d, ceil: %d, floor: %d, rem: %d\n", len, p, ceil_size, floor_size, rem);
#endif

    if (i < rem){
        start = i * ceil_size;
        end = start + ceil_size;
    }
    else{
        start = rem * ceil_size + (i-rem) * floor_size;
        end = start + floor_size;
    }
    return std::pair<int, int>(start, end);
}

/**
 * @param c A cell of the lcs matrix
 * @returns The index of the diagonal the cell belongs to
 */
int cell_diag(cell c){
    return c.first + c.second;
}

/**
 * @param c A cell of the lcs matrix
 * @returns The index of the cell w.r.t. the diagonal it belongs to,
 * from top to bottom
 */
int cell_diag_index(cell c){
    return std::min(c.first, N-c.second-1);
}

/**
 * @param c A cell of the lcs matrix
 * @returns The index of the processor assigned to it
 * Returns -1 if the cell is invalid
 */
int cell_proc(cell c){
    if(c.first < 0 || c.second < 0 || c.first >= M || c.second >= N){
        return -1;
    }
    int d = cell_diag(c);
    int len = diag_length(d);
    int pos = cell_diag_index(c);
    int p = std::min(len, P);
    int ceil_size = ceil(((float)len)/p);
    int floor_size = floor(((float)len)/p);
    int rem = len % p;

#ifndef NDEBUG
    //printf("cell (%d, %d), diag %d, len %d, pos %d, ceil %d, floor %d, rem %d\n", c.first, c.second, d, len, pos, ceil_size, floor_size, rem);
#endif

    if (pos < ceil_size * rem)
        return floor(((float) pos) / ceil_size);
    else
        return floor(((float)(pos - rem)) / floor_size);
}

/**
 * @param i The index of a processor
 * @returns A vector of cells assigned to the given processor
 */
std::vector<cell> matrix_elements(int i){

    std::vector<cell> v;
    // If there are more processors than elements, ignore processor
    // This is correct, since no diagonal is greater than M
    if(i >= M)
        return v;

    for(int d = i; d < N+M-1-i; d++){
        std::pair<int, int> indices = diag_start_end(d, i);
        for(int e = indices.first; e < indices.second; e++){
            int x = std::max(0, d-N+1) + e;
            int y = std::min(d, N-1) - e;
            v.push_back(cell(x, y));
        }
    }
    return v;
}
