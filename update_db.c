/* gets one line from a file */
I get_line(FILE *f, buf[], I length);

/*	fills one stucture from a file */
book make_structure(FILE *f, I id);

I add_note(FILE *db, book note);

/////////////////////////////////////////////////////////////////////////////////////////

/*	adds one note into db from std input */
I update_db(FILE *db)
{
	book note;
	FILE *text;
	C file_name[30];

	if (menu("console? [y/n] ") == 'y') {
		ask(db, 1);
		add_note(db, ask(db, 1));
	}
	else {
		O("name of input file?\n");
		scanf("%s", file_name);
		if (access(argv[2], F_OK) == -1) {
			O("no such file\n");
			R 0;
		}
		fill_db(db, file_name);
	}

	R 1;
}