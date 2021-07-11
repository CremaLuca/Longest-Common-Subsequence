# Longest Common Subsequence problem

### Definition:
Let $X = (x_1, \ldots, x_m)$ and $Y = (y_1, \ldots, y_n)$ be two sequences, where each $x_i, y_i \in$ alphabet $\Sigma$. We say that a sequence $Z = (z_1, \ldots, z_t)$ is a subsequence of $X$ if there exists a strictly increasing sequence $(i_1, \ldots, i_t)$ of indices of $X$ such that $z_j = x_{i_j}$, for all $1 \le j \le t$. The Longest Common Subsequence (LCS) problem consists in finding a common subsequence $Z$ of *both* $X$ and $Y$, of maximum length.
Without loss of generality, we assume $m \le n$ in the following. We first describe a sequential algorithm for the LCS problem, and then we move to the more interesting parallel case, for which we propose an algorithm that borrows its structure from the sequential one.
## Sequential algorithm
There's a well-known algorithm based on dynamic programming, that we propose here for the sequential case, which exploits the optimal substructure of the problem. Let $M$ be an $m \times n$ matrix, where entry $M[i, j]$ represents the length of an LCS of the sequences $X_i$ and $Y_j$, where $X_i$ is the $i$-th prefix of $X$, and similarly for $Y_j$. It holds that: $$M[i, j] = \begin{cases}0 & \text
{if $i = 0$ or $j = 0$}  \\M[i-1, j-1] & \text{if $i, j > 0$ and $x_i = y_j$} \\\max(M[i, j-1], M[i-1, j])  & \text{if $i, j > 0 and }\end{cases}$$

## Parallel algorithm

Based on independence of data between diagonals.

### Optimal execution order

We have to assign an order of execution for every cell in the LCS matrix. The assignment we are looking for has to:
- maximize concurrent computation
- minimize communication costs

To maximize concurrent computation we can look at the CDAG of the matrix, where we compute each cell that has required variables ready as soon as possible; we'll notice how every diagonal is a level of the greedy schedule because every element is dependent from two elements of the previous diagonal. We can deduce that the best assignment for parallelism is some permutation of the sequence of

- $\lceil L/P \rceil$ of processor i for  $0 \leq i < L \mod P$
- $\lfloor L/P \rfloor$ of processor j for $L \mod P \leq j < P$

Where L is the length of the diagonal and $P \leq L$.

We now focus on the permutation of forementioned sequence to minimize the communcation between processors. The intuitive way to achieve this is by having processors assigned to contiguous cells of the diagonal of the matrix and in the same order for each diagonal, this way we increase the probability for each processor to have the required variables from the previous diagonal already stored in its memory. Each processor will then have assigned the cells:
- if $i < L_d \mod P$
	- from $i \lfloor \frac{L_d}{P} \rfloor$
	- to $(i +1)\lfloor \frac{L_d}{P} \rfloor$
- otherwise
	- from $(L_d \mod P) \cdot \lfloor \frac{L_d}{P} \rfloor + (i - (L_d \mod P)) * \lceil \frac{L_d}{P} \rceil$
	- to $(L_d \mod P) \cdot \lfloor \frac{L_d}{P} \rfloor + ((i+1) - (L_d \mod P) ) * \lceil \frac{L_d}{P} \rceil$

Or more intuitively using an algorithm

```py
def diag_length(d: int):
	return min(d+1, N, M, M+N-1-d)

def diagonal_start_end(d: int, i: int):
	"""
	Parameters:
		- d: int
			Diagonal index.
		- i: int
			Processor index.
	Returns:
		A tuple with the first index of the cells for diagonal d
		assigned to processor i and the last index.
    """
    # Calc length of the diagonal
	L_d = diag_length(d)
	# Avoid assigning a cell to more than one proecessor
	p = min(P, L_d)
	# Number of cells for the first L_d % p processors
	ceil_size = math.ceil(L_d / p)
	# Number of cells for the other p - L_d % p processors
	floor_size = math.floor(L_d / p) # or ceil_size - 1
	rem = L_d % p
	if i < rem:
		start = i * ceil_size 
		end = start + floor_size 
	else:
		start = rem * ceil_size + (i-rem) * floor_size 
		end = start + floor_size
	return (start, end)
```
---
### Number of messages

A performance metric we use for the assignment is the number of messages exchanged by the processors. The exact measure for variables N, M and P would be too hard to bear, so we give an upper bound, P = min{N, M}.

When P=min{N, M} every cell of each diagonal is assigned to a different processor.


### List of cells per processor

Remember that:
- In an NxM matrix there are N+M-1 diagonals.
- The diagonal $d$ has length $L_d = min(d, N, M, N+M-1-d)$

On the diagonal $d$ the i-th element is 
- if d < N: `(x: i, y: d-i)`
- else the diagonal starts from another row so `(x: d-N+1-i, y: N-1-e)`

The algorithm to determine the list of elements of the whole matrix then is:
```py
def matrix_elements(i:int):
	"""
	Parameters:
		- i: int
			Processor index.
	Returns:
		A list of cells of the LCS matrix assigned to processor i.
	"""
	elements = []
	# If there are too many processor this one doesn't do anything
	if i > N or i > M:
		return elements
	# Eg. Processor 1 (starting from 0) will never be in the first and last diagonal
	for d in range(i, N+M-1-i):
		start, end = diagonal_start_end(d, i)
		for e in range(start, end):
			# e if d < N and the diagonal starts from the top, d-N+1+e otherwise
			x = max(0, d-N+1) + e
			# d-e if d < N and the diagonal starts from the top, N-1-e otherwise
			y = min(d, N-1) - e
			elements.append((x,y))
```
---
### Index of cell in diagonal

It will be useful later to know in which diagonal and which index a given cell is.
Given a cell coordinates (i, j) the diagonal is given by `i+j`, while the index in the diagonal is given by the row `i`if  d < N and `(N-d-1)+i` otherwise (or `(N-i-j-1)+i`=`N-j-1`).

```py
def cell_diag(i: int, j: int):
	"""
	Parameters:
		- i, j: int
			Coordinates of a matrix cell.
	Returns:
		The index of the diagonal where the cell belongs.
	"""
	return i+j

def cell_diag_index(i: int, j: int):
	"""
	Parameters:
		- i, j: int
			Coordinates of a matrix cell.
	Returns:
		The index of the cell w.r.t. the diagonal it belongs to.
	"""
	return min(i, N-j-1)
```

### Find which processor a cell is assigned to

```py
def cell_proc(i: int, j: int):
	"""
	Parameters:
		- i, j: int
			Coordinates of a matrix cell.
	Returns:
		The processor assigned to the given cell.
	"""
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
```

### Send computed values

Instead of keeping the matrix of whom is what we can devise a constant-time algorithm to find where to send the computed values. Easily enough there are only two cases of a value sent to the next diagonal: from (i, j) to (i+1, j) and to (i, j+1). The case from (i, j) to (i+1, j+1) never happens because (i+1, j+1) does not belong to the next diagonal $(d_{(i+1, j+1)}=d_{(i,j)}+2)$.

We can use the previous algorithm `cell_proc(i, j)` to check whether the cells $(i+1, j)$ and $(i, j+1)$belong to current processor and in case of a negative answer we send it to the respective process.

We know for sure that the cell on the right can belong either to the current processor or the previous one, and the one below to the next one, but we couldn't find an usage of this information to improve the algorithm speed.

```py
def send(x: int, y: int, i: int):
	# Send the value right if needed
	if i != 0: # p_0 never sends right
		# No need to check whether y+1 < N because only p_0 would do that
		# Can either be i or i-1
		p_right = cell_proc(x, y+1)
		if p_right != i:
			MPI_SEND(p_right)
			return # No need to send it below too
	# Send the value below if needed
	if i != P-1: # p_P-1 never sends below
		if (x + 1 < N): # Avoid out of bounds
			p_below = cell_proc(x+1, y)
			if p_below != i:
				MPI_SEND(p_below)
```
<!--stackedit_data:
eyJoaXN0b3J5IjpbNjEwNDQ5OTA3LDEwMzIyMDY0MTIsLTExOT
kyNTQ0NjksLTMzNDU5OTAwOV19
-->