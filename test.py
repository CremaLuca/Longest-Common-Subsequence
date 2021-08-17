import math
import sys
import itertools

P = 4
N = 6
M = 5


def matrix_elements(i: int):
    elements = []
    # Eg. Processor 1 will never be in the first and last diagonal
    for d in range(i, N+M-1-i):
        print(f"d: {d}")
        L_d = min(d+1, N, M, N+M-1-d)
        print(f"L_d: {L_d}")
        start = i*math.ceil(L_d/P)
        end = min(L_d, (i+1) * math.ceil(L_d/P))
        print(f"start: {start}, end: {end}")
        for e in range(start, end):
            # e if d < N and the diagonal starts from the top, d-N+1+e otherwise
            x = max(0, d-N+1) + e
            # d-e if d < N and the diagonal starts from the top, N-1-e otherwise
            y = min(d, N-1) - e
            print(f"x: {x}, y:{y}")
            elements.append((x, y))
    return elements


def get_cells(i: int):

    elements = []

    for d in range(i, N+M-1-i):
        len_d = min(d+1, N, M, M+N-1-d)
        p = min(P, len_d)
        size1 = math.ceil(len_d / p)
        size2 = math.floor(len_d / p)
        rem = len_d % p
        if i < rem:
            start = i * size1
            end = start + size1
        else:
            start = rem * size1 + (i-rem) * size2
            end = start + size2

        for e in range(start, end):
            x = max(0, d - N + 1) + e
            y = min(d, N - 1) - e
            elements.append((x, y))

    return elements


def print_matrix(matrix):
    for r in range(0, M):
        print(matrix[r])


def double_check():
    matrix = [[0 for y in range(N)] for c in range(M)]
    for p in range(0, P):
        elements = get_cells(p)
        for (x, y) in elements:
            matrix[x][y] = p
    print_matrix(matrix)


def diag_length(d: int):
    return min(d+1, N, M, M+N-1-d)


def cell_diag(i: int, j: int):
    return i+j


def cell_diag_index(i: int, j: int):
    return min(i, N-j-1)


def cell_proc(i: int, j: int):
    d = cell_diag(i, j)
    L_d = diag_length(d)
    pos = cell_diag_index(i, j)
    p = min(L_d, P)
    ceil_size = math.ceil(L_d/p)
    floor_size = math.floor(L_d/p)
    rem = L_d % p
    if pos < ceil_size * rem:
        return math.floor(pos / ceil_size)
    else:
        return math.floor((pos - rem)/ floor_size)


def diag_check():
    matrix = [[0 for y in range(N)] for c in range(M)]
    for x, y in [(a, b) for a in range(M) for b in range(N)]:
        p = cell_proc(x, y)
        matrix[x][y] = p
    print_matrix(matrix)


if __name__ == "__main__":
    diag_check()
