#include "utils.h"

int N, M, P;

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
