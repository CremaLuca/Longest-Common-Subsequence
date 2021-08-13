#ifndef UTILS_H
#define UTILS_H

#include <algorithm>
#include <cmath>
#include <vector>

#define NDEBUG

typedef std::pair<int, int> cell;

int diag_length(int d);
std::pair<int, int> diag_start_end(int d, int i);
int cell_diag(cell c);
int cell_diag_index(cell c);
int cell_proc(cell c);
std::vector<cell> matrix_elements(int i);

struct hash_pair {
    template <class T1, class T2>
    size_t operator()(const std::pair<T1, T2>& p) const
    {
        auto hash1 = std::hash<T1>{}(p.first);
        auto hash2 = std::hash<T2>{}(p.second);
        return hash1 ^ hash2;
    }
};

const int COMPUTE_LCS_TAG = 1;
const int STORE_LCS_TAG = 2;
const int STOP_TAG = 3;

#endif // UTILS_H


