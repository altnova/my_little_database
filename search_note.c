/*	checks for subject matching in current note */
/*	returns 1 or 0	*/ 
I match_all(book note, H yr, S publ, I pg, S ttl, S nm, S surnm, S patr, S subj);

/////////////////////////////////////////////////////////////////////////////////////////

/* 	searches for current fields in notes	*/
void search_note(FIlE *db, H yr, S publ, I pg, S ttl, S nm, S surnm, S patr, S subj)
{
	rec note;
	while (fread(&note, SZ(book), 1, db)) 
		if (note.deleted && match_all(note, yr, publ, pg, ttl, nm, surnm, patr, subj))
			rec_print(note);
}
