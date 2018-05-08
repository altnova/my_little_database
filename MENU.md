# MENU SYSTEM

0 - MAIN MENU
--------------
```
1. Search records           -> 1
2. Add record
3. Delete record
4. Edit record
5. Display record
6. Display all records
7. Database stats
8. Vacuum database

0. Exit program
```

1 - SEARCH
--------------------------------------------------------------
```
1. By Year                -> 1.1
2. By Title               -> 1.1
3. By Author              -> 1.1
4. By Subject             -> 1.1

0. To main menu           -> 0
```

1.1 - SEARCH RESULTS
--------------------------------------------------------------
```
Searching by: Author
Query:        pelevin [hidden by default]

  SEARCH HITS: [hidden by default]
  
  (130) CHAPAEV AND VOID        Pelevin   1996  EKSMO    423pp
  (131) OMON RA                 Pelevin   1992  EKSMO    543pp
  (132) THE LIFE OF INSECTS     Pelevin   1993  EKSMO    303pp
  (133) EMPIRE V                Pelevin   2006  EKSMO    123pp
  (134) GENERATION P            Pelevin   1999  EKSMO    343pp

Enter new query (blank to abort): _           -> 1 or 1.2

[ERR: Not a number]
[ERR: Query too short (3 chars min.)]
```

2 - ADD RECORD
--------------------------------------------------------------
```
YEAR: _
PUBLISHER: _
PAGES: _
TITLE: _
NAME: _
SURNAME: _
YEAR: _
SUBJECT: _

[OK: Record created with ID 135. Create another (Y/N)?]         -> 2 or 0 
[ERR: Not a number]
[ERR: Input too long]
```

3 - DELETE RECORD
--------------------------------------------------------------
```
Enter Record ID (blank to abort): _

[OK: Record ID nnn deleted. Delete another (Y/N)?]              - 3 or 0
[ERR: Not a number]
[ERR: No such record]
```

4 - EDIT RECORD
--------------------------------------------------------------
```
Enter Record ID (blank to abort): _

  RECORD 130

  1. Title:       CHAPAEV AND VOID
  2. Author:      Pelevin V.O.
  3. Year:        1996
  4. Publisher:   EKSMO
  5. Pages:       423
  6. Subject:     Pyotr Pustota is a poet who takes up a stolen identity and meets a strange man named Vasily Chapayev.
  
  0. Done.                            -> 0

Select field to edit: _

Old value: CHAPAEV AND VOID
New value: _

[OK: Record ID nnn updated. Continue editing (Y/N)?]        -> 4 or 0
[ERR: Not a number]
[ERR: No such record]
[ERR: Input too long]

```

5 - DISPLAY RECORD
--------------------------------------------------------------
```
Enter Record ID (blank to abort): _

TODO
```

6 - DISPLAY ALL RECORDS
--------------------------------------------------------------
```
1. Sorted by ID
2. Sorted by Year
3. Sorted by Title
4. Sorted by Author

TODO
```

7 - DATABASE STATS
--------------------------------------------------------------
```
Data file:    ~/arina/books.dat (800 bytes)
Index file:   ~/arina/books.idx (64 bytes)
Records:      2
Deleted:      0
Last ID:      2

Press any key to return to main menu.                     -> 0
```

8 - VACUUM DATABASE
--------------------------------------------------------------
```
[OK: Successfuly purged 2 deleted records.]

Press any key to return to main menu.                     -> 0
```
