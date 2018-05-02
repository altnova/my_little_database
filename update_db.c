/*	fills one stucture from a file */
book make_structure(FILE *f, I id);

/* 	struct		-->		..[db] 	*/	
I add_note(FILE *db, book note);

/////////////////////////////////////////////////////////////////////////////////////////

/*	adds one note into db from std input */
I update_db(FILE *db)
{
	book note;
	FILE *text;
	C file_name[30];

	if (menu("console? [y/n] ") == 'y') 
		note = add_note(db, ask(db, 0));
	
	else {
		O("name of input file?\n");
		scanf("%s", file_name);
		getchar();

		if (!fill_db(db, file_name)) {
			O("no such file\n");
			R 0;
		}

	}
	note.book_id = ++last_id(db);
	R 1;
}