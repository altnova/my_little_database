/*	finds required note and sets note.deleted = 1 */
void rec_delete(FILE *db, I id);

/////////////////////////////////////////////////////////////////////////////////////////

I rec_del(FILE *db, I id)
{
	if (menu("are you sure? [y/n]  ") == 'y')
		rec_delete(db, id);
	
	R 1;
}

