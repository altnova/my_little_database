/* gets one line from a file */
I get_line(FILE *f, C buf[], I length);

/* ..[txt]..	-->		struct 	*/
book make_structure(FILE *f);

/* adds one note into db */
I add_note(FILE *db, book note);

/////////////////////////////////////////////////////////////////////////////////////////

/* fills db with info from file_name */
void fill_db(FILE *db, S file_name)
{
	book note;
	FILE *text = fopen(file_name, "r");
	fseek(db, 0, SEEK_END);
	while (add_note(db, make_structure(file_name)));
	fclose(text);
}