/* gets one line from a file */
I get_line(FILE *f, C buf[], I length);

/* ..[txt]..	-->		struct 	*/
book rec_make(FILE *f);

/* adds one note into db */
I rec_add(FILE *db, book note);

/////////////////////////////////////////////////////////////////////////////////////////

/* fills db with info from file_name */
I fill_db(FILE *db, S file_name)
{
	rec note;

	if (!access(file_name, F_OK ) != -1)
		R 0;
	FILE *text = fopen(file_name, "r");
	fseek(db, 0, SEEK_END);

	while (rec_add(db, rec_make(text, db), ftell(db)));
	
	fclose(text);
	R 1;
}