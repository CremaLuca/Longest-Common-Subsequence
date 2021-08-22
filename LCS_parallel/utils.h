#ifndef UTILS_H
#define UTILS_H

#include <algorithm>
#include <cmath>
#include <vector>
#include <stdio.h>

#define NDEBUG

extern int N, M, P;

typedef std::pair<int, int> cell;

/**
 * @param d Index of the diagonal
 * @returns Length of the given diagonal
 */
inline int diag_length(int d){
    /*
    int arr[] = {d+1, M, M+N-1-d};
    return *std::min_element(arr, arr + 3);*/

    if(d <= M-1)
        return d+1;
    else if(d <= N-1)
        return M;
    else
        return M+N-1-d;
}

/**
 * @param c A cell of the lcs matrix
 * @returns The index of the processor assigned to it
 */
inline int cell_proc(cell c){
    /*
    if(c.first < 0 || c.second < 0 || c.first >= M || c.second >= N){
        return -1;
    }*/
    int d = c.first + c.second; //cell_diag
    int len = diag_length(d);
    int pos = std::min(c.first, N-c.second-1); //cell_diag_index
    int p = std::min(len, P);
    int ceil_size = ceil(((float) len)/p);
    int floor_size = floor(((float) len)/p);
    int rem = len % p;

#ifndef NDEBUG
    //printf("cell (%d, %d), diag %d, len %d, pos %d, ceil %d, floor %d, rem %d\n", c.first, c.second, d, len, pos, ceil_size, floor_size, rem);
#endif

    if (pos < ceil_size * rem)
        return floor(((float) pos) / ceil_size);
    else
        return floor(((float)(pos - rem)) / floor_size);
}


std::vector<cell> matrix_elements(int i);

struct hash_pair {
    template <class T1, class T2>
    size_t operator()(const std::pair<T1, T2>& p) const
    {
        auto hash1 = std::hash<T1>{}(p.first);
        auto hash2 = std::hash<T2>{}(p.second);
        return size_t(hash1) << 32 | hash2;
    }
};


/**
 * @param c A cell of the lcs matrix
 * @returns The index of the diagonal the cell belongs to
 */
inline int cell_diag(cell c){
    return c.first + c.second;
}

/**
 * @param c A cell of the lcs matrix
 * @returns The index of the cell w.r.t. the diagonal it belongs to,
 * from top to bottom
 */
inline int cell_diag_index(cell c){
    return std::min(c.first, N-c.second-1);
}


/**
 * @param d Index of the diagonal
 * @param i Index of the processor
 * @returns A pair (s, e) indicating the starting/ending cell on
 * the diagonal d assigned to processor i. Index e is exclusive,
 * while index s in inclusive
 */
inline std::pair<int, int> diag_start_end(int d, int i){
    int len = diag_length(d);
    int p = std::min(P, len);
    int ceil_size = ceil(((float) len)/p);
    int floor_size = floor(((float) len)/p);
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



const int COMPUTE_LCS_TAG = 1;
const int STORE_LCS_TAG = 2;
const int STOP_TAG = 3;

#endif // UTILS_H


