# MENU SYSTEM

0 - MAIN MENU
--------------
1. Search                 -> 1
2. Add record
3. Delete record
4. Edit record
5. Show record
6. Show all records
7. Database stats
0. Exit program

1 - SEARCH
--------------------------------------------------------------
1. By Year                -> 1.1
2. By Title               -> 1.1
3. By Author              -> 1.1
4. By Subject             -> 1.1
0. To main menu           -> 0

1.1 - SEARCH RESULTS
--------------------------------------------------------------
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

2 - ADD RECORD
--------------------------------------------------------------
YEAR: _
PUBLISHER: _
PAGES: _
TITLE: _
NAME: _
SURNAME: _
YEAR: _
SUBJECT: _

[OK: Record created with ID 135. Press any key to return to main menu.] -> 0
[ERR: Not a number]
[ERR: Input too long]

7 - DATABASE STATS
--------------------------------------------------------------
Data file:    ~/arina/books.dat (800 bytes)
Index file:   ~/arina/books.idx (64 bytes)
Records:      2
Deleted:      0
Last ID:      2

Press any key to return to main menu.                     -> 0
