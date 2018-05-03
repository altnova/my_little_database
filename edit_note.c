/* 	rec n2 complements rec n1	*/
rec rec_merge(rec n1, rec n2)

void rec_search(FILE *db, H yr, S publ, I pg, S ttl, S nm, S surnm, S patr, S subj);

/////////////////////////////////////////////////////////////////////////////////////////

/*	edits wanted note in db	*/
I edit_note(FILE *db, I id, rec record)
{
	I ptr;
	rec note;
	if (menu("are you sure? [y/n]  ") == 'y') {
		ptr = get_pos_by_id(db, id);
		fseek(db, ptr, SEEK_SET);
		fread(&record, SZ(rec), 1, db);
		O("please, enter correcting fields\n");

		note = rec_merge(rec_ask(db, 1), record);
		note.rec_id = record.rec_id;
		rec_print(note);

		if (menu("like this? [y/n]  ") == 'y')
			add_note(db, note, ptr);
	}

	R 1;
}