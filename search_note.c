/*	checks for subject matching in current note */
/*	returns 1 or 0	*/ 
I match_all(book note, H yr, S publ, I pg, S ttl, S nm, S surnm, S patr, S subj);

/////////////////////////////////////////////////////////////////////////////////////////

/* 	searches for current fields in notes	*/
book* search_note(FIlE *db, H yr, S publ, I pg, S ttl, S nm, S surnm, S patr, S subj)
{
	I i = 0;
	book *note = calloc(2, SIZE(book));
	while (fread(&note[i], SIZE(book), 1, db))
		if (match_all(note[i], yr, publ, pg, ttl, nm, surnm, patr, subj)) {
			note = realloc(note, i + 3);
			note[++i + 1] = 0;
		}
	R note;
}
