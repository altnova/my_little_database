/*	checks for subject matching in current note */
/*	returns 1 or 0	*/ 
I match_all(book note, H yr, S publ, I pg, S ttl, S nm, S surnm, S patr, S subj);

book* search_note(FILE *db, H yr, S publ, I pg, S ttl, S nm, S surnm, S patr, S subj)

/////////////////////////////////////////////////////////////////////////////////////////

/*	edits wanted note in db	*/
I edit_note(FILE *db)
{
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

		note = combine(ask_note(db, 1), notes[num]);
		note.book_id = notes[num].book_id;
		print_note(note);
		if (menu("like this? [y/n]  ") == 'y')
			if (!add_note(db, note, get_pos_by_id(db, notes[num].book_id)))
				O("oops\n");
	}
	free(notes);
	R 1;
}