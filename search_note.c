/*	checks for subject matching in current note */
/*	returns 1 or 0	*/ 
I match_all(book note, H yr, S publ, I pg, S ttl, S nm, S surnm, S patr, S subj);

/////////////////////////////////////////////////////////////////////////////////////////

/* 	searches for current fields in notes	*/
book* search_note(FIlE *db, H yr, S publ, I pg, S ttl, S nm, S surnm, S patr, S subj)
{
	I i = 0;
	book *note = calloc(1, SIZE(book));

	while (fread(&note[i], SIZE(book), 1, db))
		if (note[i].deleted && match_all(note[i], yr, publ, pg, ttl, nm, surnm, patr, subj)) {
			note = realloc(note, ++i + 1);
			note[i] = 0;
		}
		
	note[i] = 0;
	R note;
}
