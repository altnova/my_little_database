/* 	searches for current fields in notes and prints them */
void search_note(FILE *db);

/*	finds required note and sets note.deleted = 1 */
void delete_note(FILE *db, I id);

/////////////////////////////////////////////////////////////////////////////////////////

I del(FILE *db)
{
	I num = 0;
	book *notes, note = ask_note(db, 1);
	notes = search_note(db, note.year, note.publisher, 
						note.page, note.title, note.name, 
						note.surname, note.patronymic, note,subject);

	if (notes[0])
		print_notes(notes);
	else {
		O("nothing found\n");
		R 0;
	}

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