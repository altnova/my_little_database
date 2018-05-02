
/*	prints one note */
void print_note(book note);

/* gets one line from a file */
I get_line(FILE *f, C buf[], I length);

I match_all(book note, H yr, S publ, I pg, S ttl, S nm, S surnm, S patr, S subj);

book* search_note(FILE *db, H yr, S publ, I pg, S ttl, S nm, S surnm, S patr, S subj)

/////////////////////////////////////////////////////////////////////////////////////////

/*	edits wanted note in db	*/
I edit_note(FILE *db)
{
	I ptr;
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
	if (menu("are you sure? [y/n]  ") == 'y') {
		O("please, enter correcting fields\n");
		ptr = get_pos_by_id(db, notes[num].book_id);
		fseek(db, ptr, SEEK_SET);

		note = ask_note(db, 1);
		note.book_id = notes[num].book_id;
		note.deleted = 0;

		if (!note.year)
			note.year = notes[num].year;
		if (!note.publisher)
			note.publisher = notes[num].publisher;
		if (!note.pages)
			note.pages = notes[num].pages;
		if (!note.title)
			note.title = notes[num].title;
		if (!note.name)
			note.name = notes[num].name;
		if(!note.surname)
			note.surname = notes[num].surname;
		if (!note.patronymic)
			note.patronymic = notes[num].patronymic;
		if (!note.subject)
			note.subject = notes[num].subject;

		fwrite(note, SIZE(book), 1, db);
	}
	R 0;
}