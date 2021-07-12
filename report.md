# Longest Common Subsequence problem

### Definition:
Let $X = (x_1, \ldots, x_m)$ and $Y = (y_1, \ldots, y_n)$ be two sequences, where each $x_i, y_i \in$ alphabet $\Sigma$. We say that a sequence $Z = (z_1, \ldots, z_t)$ is a subsequence of $X$ if there exists a strictly increasing sequence $(i_1, \ldots, i_t)$ of indices of $X$ such that $z_j = x_{i_j}$, for all $1 \le j \le t$. The Longest Common Subsequence (LCS) problem consists in finding a common subsequence $Z$ of *both* $X$ and $Y$, of maximum length.
Without loss of generality, we assume $m \le n$ in the following. We first describe a sequential algorithm for the LCS problem, and then we move to the more interesting parallel case, for which we propose an algorithm that borrows its structure from the sequential one.
## Sequential algorithm
There's a well-known algorithm based on dynamic programming, that we propose here for the sequential case, which exploits the optimal substructure of the problem. Let $M$ be an $m \times n$ matrix, where entry $M[i, j]$ represents the length of an LCS of the sequences $X_i$ and $Y_j$, where $X_i$ is the $i$-th prefix of $X$, and similarly for $Y_j$. It holds that: $$M[i, j] = \begin{cases}0 & \text
{if $i = 0$ or $j = 0$}  \\M[i-1, j-1] & \text{if $i, j > 0$ and $x_i = y_j$} \\\max(M[i, j-1], M[i-1, j])  & \text{if $i, j > 0$ and $x_i \ne y_j$}\end{cases}$$
From this simple recurrence relation, it's easy to design a sequential algorithm that solves the LCS problem, filling each row one at a time. It follows that the length of an LCS is saved in $M[m-1, n-1]$. Since by computing the entries of $M$ row by row, the algorithm needs only both the current row and the previous row, we can reduce the space requirements to $\theta(n)$, instead of $\theta(mn).$ If, however, not only the length of an LCS is required, but also the actual subsequence, we need the whole matrix $M$. To construct an LCS of sequences $X_i$, $Y_j$ it's enough to start at entry $M[i-1, j-1]$ and follow at each step the previous entry which led to the computation of the current entry.

## Parallel algorithm

We will exploit the previous recurrence relation, trying to find a way to parallelize the computation. Let us first define what we mean by principal diagonal of $M$.
**Definition:** The $M$'s principal diagonal of index $d$, for $0 \le d \le m + n -2$, is the ordered set of entries$$D(d) =\begin{cases}\{M[0, d], M[1, d-1], \ldots,  M[d, 0])\} & \text
{if $0\le d < m$}  \\\{M[0, d], M[1, d-1], \ldots,  M[m-1, d-m+1])\} & \text{if $m \le d < n$} \\\{M[d-n + 1, n-1], M[N-d+2, n-2], \ldots,  M[m-1, d-m+1])\}  & \text{if $d \ge n$}\end{cases}$$
Each entry in $D(d)$ will depend only on entries belonging to $D(d-1)$ and D($d-2)$. In fact each element depends only on three elements from the two previous principal diagonals. This suggests a way to parallelize our initial algorithm: by looking at the CDAG of the computation, each diagonal is a level of the greedy schedule. Hence each entry in each diagonal can be computed in parallel, as long as entries from the previous diagonals have already been computed. From the previous definition, we have $$L(d) = |D(d)| =\begin{cases}d+1 & \text
{if $0\le d < m$}  \\m & \text{if $m \le d < n$} \\m+n-1-d & \text{if $d \ge n$}\end{cases}$$
or, more concisely, $L(d) = \min\{d+1, m, m+n-1-d\}$. 

### Optimal execution order

We have to assign an order of execution to compute every entry in the LCS matrix. The assignment we are looking for has to:
- maximize concurrent computation
- minimize communication costs

To maximize concurrent computation we can look at the CDAG of the matrix, where we compute each cell that has required variables ready as soon as possible; let $P_{\text{max}} > 0$ processors at our disposal. Let's see how many processors we need to assign to a given principal diagonal $d$ whose length is $L= L(d)$. Notice that if$L < P_{\text{max}}$, $P_{\text{max}}-L$ processors will not work at all, since each entry can be computed in parallel by $L$ processors. So we put $P = \min\{L(d), P_{\text{max}}\}$. Thus we can assign processors as follows:

- $\lceil L/P \rceil$ cells to  processor $i$ for  $0 \leq i < L \mod P$
- $\lfloor L/P \rfloor$ cells to processor $j$ for $L \mod P \leq j < P$

We now focus on the permutations of this sequence, in order to minimize the communication between processors. The intuitive way to achieve this is by having processors assigned to contiguous cells of the diagonal of the matrix and in the same order for each diagonal: this way we increase the probability for each processor to have the required variables from the previous diagonal already stored in its memory. Let $D$ be the principal diagonal of index $d$; processor $i$ will have to compute entries from $D[s]$ to $D[e-1]$, where:
$$\begin{align*}
s &=\begin{cases}i \Bigl \lfloor \frac{L(d)}{P}\Bigr\rfloor  \text
{\quad if $i < L(d) \mod P$}  \\\\(L(d) \mod P) \cdot\Bigl \lfloor  \frac{L(d)}{P} \Bigr\rfloor + (i - (L(d) \mod P)) \cdot \Bigl \lceil  \frac{L(d)}{P} \Bigr\rceil & \text{otherwise} \end{cases}\\\\
e &=\begin{cases} s + \Bigl \lfloor  \frac{L(d)}{P}\Bigr\rfloor & \text
{if $i < L(d) \mod P$}  \\\\s+ \Bigl\lceil \frac{L(d)}{P} \Bigr\rceil & \text{otherwise} \end{cases}
\end{align*}
$$

Or more intuitively using an algorithm:

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
	# Avoid unnecessary processors
	p = min(P, L_d)
	# Number of cells for the first L_d % p processors
	ceil_size = math.ceil(L_d / p)
	# Number of cells for the other p - L_d % p processors
	floor_size = math.floor(L_d / p) # or ceil_size
	rem = L_d % p
	if i < rem:
		start = i * ceil_size 
		end = start + floor_size 
	else:
		start = rem * ceil_size + (i-rem) * floor_size 
		end = start + floor_size
	return (start, end)
```

### Bound on the number of messages
Let us define $P(i, j)$ as the index of the processor assigned to entry $(i, j)$, according to the previous scheme.

**Remark:** It's quite easy to see that $P(i, j)$, during the computation of entry $(i, j)$,  already has the value of cell $(i-1, j-1)$ stored in its memory, for $i, j \ge 1$. First notice that at least one of $(i, j-1)$ or $(i-1, j)$ is assigned to $p$:  in fact, let's say cell $(i, j)$ lies on diagonal $d$; then if $L(d-1) \le L(d)$, it follows that $P(i, j) = P(i, j-1)$; however, if $L(d-1) > L(d)$, $P(i, j) = P(i-1, j)$. Hence, in either case, cell $(i-1, j-1)$ is known to $p$, since its value was fetched by $p$ in the previous diagonal.

A performance metric we use for the assignment is the number of messages exchanged by the processors. The exact measure for variable $n, m$ and $P_{\text{max}}$ is hard to obtain from analytical considerations, but we can give an upper bound: clearly we can assume $P_{\text{max}} = \min\{n, m\} = m$ since no diagonal will be longer than $m$.
In this case *every* cell of *each* diagonal is assigned to a *different* processor. By the remark, it's easy to prove that $P(i, j) = P(i, j-1) \ne P(i-1, j)$ if $i+j \le n-1$ and $P(i, j) = P(i-1, j) \ne P(i, j-1)$  if $i+j \ge n$.  Hence: $$\text{messages exchanged} = \sum_{i = 1}^{m-1}\sum_{j=1}^{n-1}1 + \underbrace{m-1}_{\text{first column}} = n(m-1) = \theta(nm)$$

### Computing the list of entries assigned to a given processor
Each processor needs to know which entries to compute. We make use of the observations above, i.e. the procedure `diagonal_start_end`. Notice that processor $i$ will never appear on principal diagonals $d$ s.t. $d < i$ or $d \ge n+m-1-i$, since in both cases the length of the diagonal is $\le i$. Also since no diagonal has length $\ge\min\{m, n\} = m$, we need $i < m$.
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
	
	# If there are too many processor this one doesn't do anything
	if i >= min(M, N):
		return []
	elements = []
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
### Some useful functions

Given entry $(i, j)$, it will be useful for the following to know which diagonal index $d$ it corresponds, as well as its position relative to $D(d)$, which we call $\text{pos}(i, j)$.
Given the coordinates $(i, j)$ the diagonal is clearly  $i+j$, while $\text{pos(i, j)}$ is given by row $i$ if  $d < n$ and $N-j-1$ otherwise: this can be condensed into $\min\{i, n-j-1\}$.

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

Another useful function is needed to compute $P(i, j)$.
It's easy to derive a formula from the previous assignment of processors to each diagonal; 
set $L = L(i+j)$ and $p = \min\{L, P_{\text{max}}\}$, $q =\lfloor L/p\rfloor$, $r=L\mod p$.
We have:$$P(i, j)=\begin{cases}\bigl\lfloor\frac{\text{pos}(i, j)}{q+1}\bigr\rfloor& \text
{if pos(i, j) $<(q+1)r$}  \\\bigl\lfloor\frac{\text{pos}(i, j)-r}{q}\bigr\rfloor & \text{otherwise}\end{cases}$$
Hence the following algorithm:

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
Each processor doesn't need to keep a copy of the matrix $P(i, j)$: to find where to send the computed values, the previous formula can be used. As we already mentioned in a remark, each processor needs to send at most $2$ values to different neighbors: we can use the previous algorithm `cell_proc(i, j)` to check whether the cells $(i+1, j)$ and $(i, j+1)$ belong to the current processor and in case of a negative answer we send their values to the proper processes.

We know for sure that the cell on the right can belong either to the current processor or the previous one, while, similarly, the one below either to the current or to the next one, but we couldn't find an usage of this information to improve the algorithm speed.

```py
def send(x: int, y: int, p: int):
	"""
	Parameters:
		- i, j: int
			Coordinates of a matrix cell.
		- p: int
			Process that makes a send
	"""
	# Send the value right if needed
	if p != 0: # p_0 never sends right
		# No need to check whether y+1 < N because only process 0 would do that
		# Can either be process p or p-1
		p_right = cell_proc(i, j+1)
		if p_right != p:
			MPI_SEND(p_right)
			return # No need to send it below too
	# Send the value below if needed
	if p != P-1: # p_P-1 never sends below
		if (i + 1 < N): # Avoid out of bounds
			p_below = cell_proc(i+1, j)
			if p_below != p:
				MPI_SEND(p_below)
```
<!--stackedit_data:
eyJoaXN0b3J5IjpbLTkzMzkwNzQ1NiwtNTAzMTk5NTY0LC04MD
cyMDU1NTUsODY4OTU1NDY1LDE2NTE0MDczMzAsNTM4NzIzNDQx
LDU4NzYxNzc5MiwtOTI2Nzk4MTM0LC0xNDAyNDYxNzIsMTc0MT
k5NTExMSwtMTg4MzEwNzU2NSwzMTMwMjg3MjAsLTg3OTI1OTI1
MCwtMjA4NTI1ODI0MiwtMTAxNTA1MDEwMywtMzE1ODQ0NzYzLC
0xMDkzODMzMDEsLTEwMTA0NDE5NTMsMTczNDcyMTE1MywtMTM5
MTIwNTcwXX0=
-->