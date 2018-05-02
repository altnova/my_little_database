#define LEN 411
#define C char
#define S char*
#define I int
#define H short
#define R return
#define SUB_LEN 140
#define NAME_LEN 50
#define MAX(a, b) {(a) > (b) ? (a) : (b);}
#define SIZE sizeof

struct Books {
	I book_id;
	C deleted;				// 0 -- deleted
	H year;
	C publisher[NAME_LEN];
	I pages;
	C title[NAME_LEN];
	C name[NAME_LEN];
	C surname[NAME_LEN];
	C patronymic[NAME_LEN]
	C subject[SUB_LEN];
} book;

///////////////////////////////////////////////////////////////////////////////////////

/*	asks user to tap something */
C menu(const C *hint);

/*	returns pointer for a note with given id or -1 */
I get_pos_by_id(FILE *db, I id);

/* 	returns last used id or -1 	*/
I last_id(FILE *db);

/*	prints one note */
void print_note(book note);

/*	prints few notes 	*/
void print_notes(book *note);

/* 	gets one line from a file */
/* 	retuns 0 if this line is too long 	*/
I get_line(FILE *f, C buf[], I length);

/* 	..[db]..	-->		struct 	*/ 
/* 	returns 0 if eof	*/
book get_note(FILE *db, I ptr);

/* ..[txt]..	-->		struct 	*/
book make_structure(FILE *f, FILE *db);

/*	checks for subject matching in current note */
/*	returns 1 or 0	*/ 
I match_all(book note, H yr, S publ, I pg, S ttl, S nm, S surnm, S patr, S subj);

/*	finds required note and sets note.deleted = 1 */
I delete_note(FILE *db, I id);

/* 	struct		-->		..[db] 	*/	
I add_note(FILE *db, book note);

/*	asks user to enter a note 	*/
book ask_note(FILE *db, I ask);

/* 	book n2 complements book n1	*/
book combine(book n1, book n2)

///////////////////////////////////////////////////////////////////////////////////////

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
	I pos, num, d;
	rewind(db);

	while(fseek(db, SIZE(book) - SIZE(I) - SIZE(C))) {
		fread(&num, SIZE(I), 1, db);
		fread(&d, SIZE(C), I, db);
		if (d) {
			pos = ftell(db) - SIZE(I) - SIZE(C);
			if (num == id)
				R pos;
		}
	}

	R -1;
}

/* 	returns last used id or -1 	*/
I last_id(FILE *db)
{
	I num, id;
	rewind(db);
	fread(&id, SIZE(I), 1, db) ? fseek(db, SIZE(book) - SIZE(I), SEEK_CUR) : R -1;

	while(fread(&nm, SIZE(I), 1, db)) {
		nm = MAX(nm, id);
		fseek(db, SIZE(book) - SIZE(I), SEEK_CUR);
	} 
	R nm;
}

void print_note(book note)
{
	printf("%s %s %s\n%s\n", note.name, note.surname, 
			note.patronymic, note.title);
	printf("%d pages  %hi %s\n%s\n\n", note.pages, 
			note.year, note.publisher, note.subject);
}

/*	prints one note */
void print_notes(book *note)
{
	I i = 0;
	while(note[i]) 
		print_note(note[i++]);
}

/* 	gets one line from a file */
/* 	retuns 0 if this line is too long 	*/
I get_line(FILE *f, C buf[], I length)
{
	C c;
	for (I i = 0; c != '\n' && i < length && !feof(f); i++) {
		getc(f, "%c", c);
		buf[i] = c;
	}

	if (i == length) 
		while (c != '\n' && !feof(f))
			getc(f, "%c", c);
	
	R i == length ? 0 : 1 && (buf[--i] = '\0');
}

/* 	..[db]..	-->		struct 	*/ 
/* 	returns 0 if eof	*/
book get_note(FILE *db, I ptr) 
{
	book note;
	if (ptr == SEEK_END)
		R 0;
	fread(&note, SIZE(book), 1, db);
	R note;
}

/* ..[txt]..	-->		struct 	*/
book make_structure(FILE *f, FILE *db)
{
	C c;
	book note;

	note.id = ++last_id(db);
	note.deleted = 0;

	fscanf(f, "%hi", &note.year);
	getc(f, "%c", &c);

	if (!get_line(f, note.publisher, NAME_LEN))
		R 0;

	fscanf(f, "%d", &note.pages);
	getc(f, "%c", &c);

	if (!get_line(f, note.title, NAME_LEN) || 
		!get_line(f, note.name, NAME_LEN) || 
		!get_line(f, note.surname, NAME_LEN) || 
		!get_line(f, note.patronymic, NAME_LEN) 
		!get_line(f, note.subject, SUB_LEN))
		R 0;

	R book note;
}

/*	checks for subject matching in current note */
/*	returns 1 or 0	*/ 
I match_all(book note, H yr, S publ, I pg, S ttl, S nm, S surnm, S patr, S subj)
{
	if (!note.deleted || (yr && yr != note.year) 
			|| (publ && strcmp(publ, note.publisher)) 
			|| (pg && pg != note.page) || (ttl && strcmp(ttl, note.title)) 
			|| (nm && strcmp(nm, note.name)) || (surnm && strcmp(surnm, note.surname)) 
			|| (patr && strcmp(patr, note.patronymic)) || (subj && strstr(note.subject), subj))
		R 0;

	R 1;
}

/*	finds a wanted note and sets note.deleted = 1 */
I delete_note(FILE *db, I id)
{
	I num = 1;

	if (fseek(db, get_pos_by_id(db, id) + SIZE(I), SEEK_SET))
		R 0;

	fwrite(&num, SIZE(C), 1, db);
	R 1;	
}
	
/* 	struct		-->		..[db]..	*/		
I add_note(FILE *db, book note, I ptr)
{
	if (note) {
		if (fseek(db, ptr, SEEK_SET))
			R 0;
		fwrite(&note, SIZE(book), 1, db);
		R 1;
	}
	else 
		R 0;
}

/*	asks user to enter a note 	*/
book ask_note(FILE *db, I ask) 
{ 
	book note;
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
		note.book_id = -1;


	}
	else {
		note.book_id = ++last_id(db);
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

/* 	book n2 complements book n1	*/
book combine(book n1, book n2)
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


