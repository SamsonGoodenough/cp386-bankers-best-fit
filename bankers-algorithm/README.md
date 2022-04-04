# Banker's Algorithm Details
## Customer Data Structure
- [x] `available` amount of each resource
- [x] `maximum` demand of each customer
- [x] amount currently `allocated` to each customer
- [x] remaining `need` of each customer

### available
A vector of length $m$ indicates the number of *available* resources of each type. If `Available[j]` equals $k$, then $k$ instances of resource type $R_j$ are available.
### maximum
An $n \times m$ matrix defines the *maximum* demand of each thread.
If `Max[i][j]` equals $k$, then thread $T_i$ may request at most $k$ instances of resource type $R_j$.
### allocated
An $n \times m$ matrix defines the number of resources of each type currently *allocated* to each thread.
If `Allocation[i][j]` equals $k$, then thread $T_i$ is currently allocated $k$ instances of resource type $R_j$.
### need
An $n \times m$ matrix indicates the remaining resource *need* of each thread. If `Need[i][j]` equals $k$, then thread $T_i$ may need $k$ more instances of resource type $R_j$ to complete its task.
Note that `Need[i][j] == Max[i][j] − Allocation[i][j]`.

## Safety Algorithm (from textbook)
1. Let `Work` and `Finish` be vectors of length $m$ and $n$, respectively.
   Initialize `Work = Available` and `Finish[i] = false` `for i = 0, 1, ..., n − 1`.

2. Find an index `i` such that both
  `Finish[i] == false` and `Need[i] ≤ Work`
  If no such `i` exists, go to step 4.

3. `Work = Work + Allocation[i]`
   `Finish[i] = true`
   Go to step 2.

4. If `Finish[i] == true` for all `i`, then the system is in a *safe state*.

## Steps to check for:
**Valid Request**
- Check that `requested <= need`
- Check that `requested <= available`
- Temporarily fill the request...
  - Save a backup of available
  - `available -= requested`
- Check if the system `isSafe`
  - if *true*...
    - `allocated += requested`
    - `need -= requested`
  - if *false*...
    - revert to backup

**Valid Release**
- Check that `released <= allocated`
- Release the resources
  - `available += released`
  - `allocated -= released`