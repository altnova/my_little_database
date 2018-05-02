/* gets one line from a file */
I get_line(FILE *f, buf[], I length);

/*	checks for subject matching in current note */
/*	returns 1 or 0	*/ 
I match_all(I ptr, H yr, S publ, I pg, S ttl, S nm, S surnm, S patr, S subj);

/* 	searches for current fields in notes and prints them */
void search_note(FILE *db);

void delete_note(FILE *db, I id);

/////////////////////////////////////////////////////////////////////////////////////////

/*	finds a wanted note and sets note.deleted = 1 */
I del(FILE *db)
{
	I num = 0;
	book *notes, note = ask_note(db, 1);
	notes = search_note(db, note.year, note.publisher, note.page, note.title, note.name, note.surname, note.patronymic, note,subject);
	print_notes(notes);
	if (notes[1]) {
		num = menu("which one? ") - 49;
		if (!notes[num]) {
			O("incorrect number\n");
			R 0;
		}
	}
	if (menu("are you sure? [y/n]  ") == 'y')
		delete_note(db, notes[num].book_id);
	R 1;
}