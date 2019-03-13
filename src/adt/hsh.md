
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
 

A hash table is initialized as a cntiguous array of numbered slots which
are populated by key/value pairs called "buckets". The table below has the 
capacity of 4 slots, 3 of which are occupied by buckets while one slot 
remains free:

0   1   2   3
--------------
B2  B1  -   B3


For both insertion and lookup operations, a fixed-width numerical 
representation of a given key (known as "hash value") is calculated
using some fast one-way operation called a "hash function". The resulting 
hash value is then mapped onto the address space of the table using modulo
operator, instantly yielding the exact index of a slot for insertion 
of a new bucket or for retrieving a value from an existing one. For example,
to insert a value for the key *k*="Alice" into an empty table HT of capacity 
*c*=4, we could use some hash function *H(k)* which returns hash values in 
unsigned 32-bit integer space. Then:

hash = H("Alice") = 122
index = hash mod c = 122%4 = 2
HT(2) <- B1(Alice, 555-11-22)

        *   
0   1   2   3
-------------
-   -   B1  -
        
Later, the same operation can be used to retrieve an existing bucket 
to obtain the value for an existing key. Thus, the main advantage of a hash 
table is constant time of insertion and lookup operations, entirely 
independent of the table size. In other words, a perfect hash table is O(1) 
which is very nice indeed.

The main caveat of hash-based indexing is theoretical impossibility of
constructing a hash function with a guarantee that no two or more different
inputs will ever produce an identical hash value. This unavoidable situation
is known as "collision". One possible method of handling collisions in a 
hash table is by chaining colliding buckets into linked lists:

0   1   2   3
--------------
B1  B2  B5  B3
|   |       |
B4  B6      B7
            |
            B8

The downside of this approach is that locating a key in a slot filled by a
long chain of buckets requires an additional linear traversal of a linked 
list. Hence, if the number of chains and their average length are allowed 
to grow infinitely, the performance of such hash table will tend to degrade
from O(1) towards O(n), negating its main benefit.

An intuitive solution to deal with excessive chaining is to simply add 
more free slots to the table. However, the expansion of the address space 
does not unroll any existing chains, and merely slows down their further 
growth at the expense of allocating additional memory. As it is hard to choose 
an optimal time and factor of growth in general case, the efficiency 
of such algorithms tends to be implementation-specific.

Our implementation takes a more general approach to deal with chains.
First, we introduce a state variable *level*, which is always a power of 
2. The *capacity* of the table (the current number of allocated slots) is 
defined as level*2, therefore the value of *level* always points to the first 
slot in the "right" part of the table. Instead of "left" and "right", we will
refer to all slots less than *level* as the *lower part* of the table, and 
slots at indexes equal or above *level* as its *upper part*. We also 
introduce an extra state variable *split*, which is 0 by default.

Let the initial *level* be 4 (the table capacity is therefore 8 slots). We 
begin by inserting 5 buckets. Lets assume the first four buckets did not
collide, but the 5th bucket has no other choice but to collide with one of 
the first four: the trick is that when we map the hash value onto the address 
space of the table, for now we only use the lower half of its address space, 
like the upper didn't exist and wasn't allocated:

S           L
0  1  2  3  4  5  6  7
-----------------------
B2 B1 B4 B3
   |
   B5 (oi!)










