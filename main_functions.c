#define LEN 411
#define C char
#define S char*
#define I int
#define H short
#define UJ unsigned long
#define R return
#define SUB_LEN 145
#define NAME_LEN 51
#define MAX(a, b) {(a) > (b) ? (a) : (b);}
#define SZ sizeof

struct books {
	UJ book_id;
	I pages;
	H year;
	C publisher[NAME_LEN];
	C title[NAME_LEN];
	C author[NAME_LEN];
	C subject[SUB_LEN];
} Books;

///////////////////////////////////////////////////////////////////////////////////////

/* 	str1 subline search with no case in str2	*/
I strstr_no_case(S str1, S str2)

/*	asks user to tap something */
C menu(const C *hint);

/*	returns pointer for a note with given id or -1 */
I get_pos_by_id(FILE *db, I id);

/* 	returns next id 	*/
I next_id(FILE *db);

/*	prints one note */
void rec_print(Books note);

/* 	gets one line from a file */
void get_line(FILE *f, C buf[], I length);

/* 	..[db]..	-->		struct 	*/ 
/* 	returns 0 if eof	*/
Books rec_get(FILE *db, I ptr);

/* ..[txt]..	-->		struct 	*/
Books rec_make(FILE *f, FILE *db);

/*	checks for subject matching in current note */
/*	returns 1 or 0	*/ 
I rec_match(Books note, H yr, S publ, I pg, S ttl, S nm, S surnm, S patr, S subj);

/*	finds required note and sets note.deleted = 1 */
I rec_delete(FILE *db, I id);

/* 	struct		-->		..[db] 	*/	
I rec_add(FILE *db, Books note);

/*	asks user to enter a note 	*/
Books rec_ask(FILE *db, I ask);

/* 	Books n2 complements Books n1	*/
Books rec_merge(Books n1, Books n2)

///////////////////////////////////////////////////////////////////////////////////////

/* 	str1 subline search with no case in str2	*/
I strstr_no_case(S str1, S str2)
{
	for (I i = 0; str1[i] != '\0'; i++) 
		if (str1[i] >= 'A' && <= 'Z')
			str1[i] += 32;
	
	for (i = 0; str2[i] != '\0'; i++) 
		if (str2[i] >= 'A' && <= 'Z')
			str2[i] += 32;

	if (strstr(str1, str2) == NULL)
		R 0;
	else
		R 1:
}

/*	asks user to tap something */
C menu(const C *hint) 
{
    C c;
    O("%s", hint);
    c = getchar();
    getchar();
    return c;
}

/*	returns pointer for a note with given id or -1 */
I get_pos_by_id(FILE *db, I id)
{
	/* что-то */
}

/* 	returns next id 	*/
I next_id(FILE *db)
{
	/* что-то */
}

void rec_print(Books note)
{
	printf("%s %s %s\n%s\n", note.name, note.surname, 
			note.patronymic, note.title);
	printf("%d pages  %hi %s\n%s\nid: %d\n\n", note.pages, 
			note.year, note.publisher, note.subject, note.rec_id);
}

/* 	gets one line from a file */
void get_line(FILE *f, C buf[], I length)
{
	C c;
	for (I i = 0; c != '\n' && i < length && !feof(f); i++) {
		getc(f, "%c", c);
		buf[i] = c;
	}

	if (i == length) 
		while (c != '\n' && !feof(f))
			getc(f, "%c", c);
	
	buf[--i] = '\0';
}

/* 	..[db]..	-->		struct 	*/ 
/* 	returns 0 if eof	*/
Books rec_get(FILE *db, I ptr) 
{
	Books note;
	if (ptr == SEEK_END)
		R 0;
	fread(&note, SZ(rec), 1, db);
	R note;
}

/* ..[txt]..	-->		struct 	*/
Books rec_make(FILE *f, FILE *db)
{
	C c;
	Books note;

	note.id = next_id(db);
	note.deleted = 0;

	fscanf(f, "%hi", &note.year);
	getc(f, "%c", &c);

	get_line(f, note.publisher, NAME_LEN);

	fscanf(f, "%d", &note.pages);
	getc(f, "%c", &c);

	get_line(f, note.title, NAME_LEN);
	get_line(f, note.name, NAME_LEN);
	get_line(f, note.surname, NAME_LEN);
	get_line(f, note.patronymic, NAME_LEN);
	get_line(f, note.subject, SUB_LEN);

	R note;
}

/*	checks for subject matching in current note */
/*	returns 1 or 0	*/ 
I rec_match(Books note, H yr, S publ, I pg, S ttl, S nm, S surnm, S patr, S subj, I id)
{
	if (id == note.rec_id)
		R 1;
	if (!note.deleted || (yr && yr != note.year) 
			|| (publ && strstr_no_case(publ, note.publisher)) 
			|| (pg && pg != note.page) || (ttl && strstr_no_case(ttl, note.title)) 
			|| (nm && strstr_no_case(nm, note.name)) || (surnm && strstr_no_case(surnm, note.surname)) 
			|| (patr && strstr_no_case(patr, note.patronymic)) || (subj && strstr_no_case(note.subject), subj))
		R 0;

	R 1;
}

/*	finds a wanted note and sets note.deleted = 1 */
I rec_delete(FILE *db, I id)
{
	I num = 1;

	if (fseek(db, get_pos_by_id(db, id) + SZ(I), SEEK_SET))
		R 0;

	fwrite(&num, SZ(C), 1, db);
	R 1;	
}
	
/* 	struct		-->		..[db]..	*/		
I rec_add(FILE *db, Books note, I ptr)
{
	if (note) {
		if (fseek(db, ptr, SEEK_SET))
			R 0;
		fwrite(&note, SZ(rec), 1, db);
		R 1;
	}
	else 
		R 0;
}

/*	asks user to enter a note 	*/
Books rec_ask(FILE *db, I ask) 
{ 
	Books note;
	note.deleted = 0;
	if (ask) {
		
		menu("year? [y/n]  ") == 'y' 		? 		(scanf("%d", &note.year) && getchar())					: note.year = 0;
		menu("publisher? [y/n]  ") == 'y' 	? 		get_line(stdin, note.publisher, NAME_LEN)			 	: note.publisher = 0;
		menu("pages? [y/n]  ") == 'y' 		? 		(scanf("%d", &note.pages) && getchar())					: note.pages = 0;
		menu("title? [y/n]  ") == 'y' 		?	 	get_line(stdin, note.title, NAME_LEN) 					: note.title = 0;
		menu("name? [y/n]" == 'y') 			? 		get_line(stdin, note.name, NAME_LEN) 					: note.name = 0;
		menu("surname? [y/n]" == 'y') 		? 		get_line(stdin, note.surname, NAME_LEN) 				: note.surname = 0;	
		menu("patronymic? [y/n]" == 'y') 	?	 	get_line(stdin, note.patronymic, NAME_LEN)			 	: note.patronymic = 0;	
		menu("subject? [y/n]" == 'y' 		? 		get_line(stdin, note.subject, SUB_LEN)) 				: note.subject = 0; 
		note.rec_id = -1;


	}
	else {
		note.rec_id = next_id(db);
		note.deleted = 0;

		O("year:\n");
		scanf("%d", &note.year);
		getchar();
		

		O("publisher:\n");
		get_line(stdin, note.publisher, NAME_LEN);
		
		O("pages:\n");
		scanf("%d", &note.pages);
		getchar();

		O("title:\n"); 
		get_line(stdin, note.title, NAME_LEN);

		O("name:\n");
		get_line(stdin, note.name, NAME_LEN);

		O("surname:\n");
		get_line(stdin, note.surname, NAME_LEN);
		
		O("patronymic:\n");
		get_line(stdin, note.patronymic, NAME_LEN);

		O("subject:\n");
		get_line(stdin, note.subject, SUB_LEN);
	}
	R note;
}

/* 	Books n2 complements Books n1	*/
Books rec_merge(Books n1, Books n2)
{
	if (!n1.year)
		n1.year = n2.year;
	if (!n1.publisher)
		n1.publisher = n2.publisher;
	if (!n1.pages)
		n1.pages = n2.pages;
	if (!n1.title)
		n1.title = n2.title;
	if (!n1.name)
		n1.name = n2.name;
	if (!n1.surname)
		n1.surname = n2.surname;
	if (!n1.patronymic)
		n1.patronymic = n2.patronymic;
	if (!n1.subject)
		n1.subject = n2.subject;
	R n1;
}

void rec_swap(FILE *db, I ptr1, I ptr2)
{
	Books a, b;
	fseek(db, ptr1, SEEK_SET);
	fread(&a, SZ(rec), 1, db);

	fseek(db, ptr2, SEEK_SET);
	fread(&b, SZ(rec), 1, db);
	fwrite(&a, SZ(rec), 1, db);

	fseek(db, ptr1, SEEK_SET);
	fwrite(&b, SZ(rec), 1, db);
}


