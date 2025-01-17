
-----------------------------
WHAT THE FUCK IS A HASH TABLE
-----------------------------

A hash table is a remarkable data structure designed for efficient 
retrieval of arbitrary data items (called "values") by a given "key"
uniquely assiciated with each value. As such, a hash table implements an 
*associative array*. For example, if keys are names of people and values
are their phone numbers, we have:

k       v
-----------------
Alice   555-11-22
Bob     555-33-12
Eve     555-18-77
 

A hash table is initialized as a dynamic array of numbered slots which
are populated by key/value pairs called "buckets". The table below has the 
capacity of 4 slots, 3 of which are occupied by buckets while one slot 
remains free:

0   1   2   3
--------------
B2  B1  -   B3


For both insertion and lookup operations, a fixed-width numerical 
representation of a given key (known as "hash value") is calculated
using a one-way operation called a "hash function". The resulting 
hash value is then mapped onto the address space of the table using modulo
operator, thus instantly yielding the exact index of a slot for insertion 
of a new bucket or retrieving a value from an existing one. For example,
to insert a value for the key *k*="Alice" into an empty table HT of the capacity 
*c*=4, we could use some hash function *H(k)* which produces hash values in 
unsigned 32-bit integer space. Then:

hash = H("Alice") = 122
index = hash mod c = 122%4 = 2
B1 {Alice, 555-11-22} -> HT[2]

        *   
0   1   2   3
-------------
-   -   B1  -
        
Later, we use the same operation to retrieve an existing bucket 
to obtain the value by a given key. Evidently, the main advantage of a hash 
table is /constant time/ of insertion and lookup operations, entirely 
independent of the table size. In other words, a well-designed hash table 
performs close to O(1).

Unfortunately, it is usually not possible to construct a "perfect" hash table.
The main caveat of hash-based indexing is theoretical impossibility of
constructing a hash function with a guarantee that no two or more different
keys will ever produce an identical hash value. This unavoidable situation
is known as "collision". One possible method of handling collisions in a 
hash table is by chaining colliding buckets into linked lists:

0   1   2   3
--------------
B4  B6  B5  B8
|   |       |
B1  B2      B7
            |
            B4

The downside of this approach is that locating a key in a slot filled by a
long chain of buckets requires an additional linear scan of a linked 
list. Hence, if the number of chains and their average length are allowed 
to grow infinitely, the performance of such hash table will drift
from O(1) towards O(n), negating its main benefit.

An intuitive solution to avoid excessive chaining is choose a hash function
that provides a suffuciently uniform distribution, and then simply allocate 
additional slots as required. However, the expansion of address space does 
nothing to unroll any existing chains, and merely slows down their further 
growth. In practice, this approach is only as efficient as the choice of an 
appropriate algorigthm of when and how many slots to add, which requires 
careful analysis and tuning for each specific non-trivial load.

Our implementation uses a more general approach to deal with chains.
We introduce a state variable *level (L)*, which is always a power of 
2. The *capacity* of the table (the current number of allocated slots) is 
always level*2. Every time the table needs to grow, the value of *level*
is doubled, and so does the table capacity. Note that the value of *level* 
always points to the first slot in the "right" part of the table. Instead 
of "left" and "right", we will refer to all slots less than *level* as 
the *lower part* of the table, and slots at indexes equal or above *level* 
as its *upper part*. We also introduce an additional state variable *split
(S)*, set to 0 by default.

The following example illustrates the basic idea of the algorithm.   

Let the initial *level* be 4 (the table capacity is therefore 8 slots). We 
begin by inserting 5 buckets. During an insertion, we map the hash value 
onto the lower half of the address space, as the upper part didn't even 
exist. Assuming the first 4 insertions did not produce any collisitions, 
we have:

S           L
0  1  2  3  4  5  6  7
-----------------------
B2 B1 B4 B3

However, the 5th bucket has no other choice but to collide with one of 
the first four - we are still using only the lower part, where there are 
no more free slots left. A collision occurs, and the 5th bucket becomes
a new head of a linked list:

S           L
0  1  2  3  4  5  6  7
-----------------------
B2 B5 B4 B3
   |
   B1 <- pushed down!

 traverse the rest of the linked list as follows: 








