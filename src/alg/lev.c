//! \file lev.c \brief levenshtein distance

/*
 * This function implements the Damerau-Levenshtein algorithm to
 * calculate a distance between strings.
 *
 * Basically, it says how many letters need to be swapped, substituted,
 * deleted from, or added to string1, at least, to get string2.
 *
 * The idea is to build a distance matrix for the substrings of both
 * strings.  To avoid a large space complexity, only the last three rows
 * are kept in memory (if swaps had the same or higher cost as one deletion
 * plus one insertion, only two rows would be needed).
 *
 * At any stage, "i + 1" denotes the length of the current substring of
 * string1 that the distance is calculated for.
 *
 * row2 holds the current row, row1 the previous row (i.e. for the substring
 * of string1 of length "i"), and row0 the row before that.
 *
 * In other words, at the start of the big loop, row2[j + 1] contains the
 * Damerau-Levenshtein distance between the substring of string1 of length
 * "i" and the substring of string2 of length "j + 1".
 *
 * All the big loop does is determine the partial minimum-cost paths.
 *
 * It does so by calculating the costs of the path ending in characters
 * i (in string1) and j (in string2), respectively, given that the last
 * operation is a substitution, a swap, a deletion, or an insertion.
 *
 * This implementation allows the costs to be weighted:
 *
 * - w (as in "sWap")
 * - s (as in "Substitution")
 * - a (for insertion, AKA "Add")
 * - d (as in "Deletion")
 *
 * Note that this algorithm calculates a distance _iff_ d == a.
 */
I levenshtein(const S string1, const S string2,
		I w, I s, I a, I d)
{
	I len1 = strlen(string1), len2 = strlen(string2);
	I *row0, *row1, *row2;
	I i, j;

	row0 = calloc(len2+1,1);chk(row0,0);
	row1 = calloc(len2+1,1);chk(row1,0);
	row2 = calloc(len2+1,1);chk(row2,0);

	DO(len2+1, row1[i]=j * a)

	DO(len1,
		I* dummy;UJ ii=i;
		row2[0] = (i + 1) * d;
		DO(len2, j=i;
			/* substitution */
			row2[j+1] = row1[j] + s * (string1[ii]!=string2[j]);
			/* swap */
			if (ii>0&&j>0&&string1[ii-1]==string2[j] &&
					string1[ii]==string2[j-1] &&
					row2[j+1]>row0[j-1] + w)
				row2[j+1] = row0[j-1] + w;
			/* deletion */
			if (row2[j+1] > row1[j+1] + d)
				row2[j+1] = row1[j+1] + d;
			/* insertion */
			if (row2[j+1] > row2[j] + a)
				row2[j+1] = row2[j] + a;)

		dummy = row0;
		row0 = row1;
		row1 = row2;
		row2 = dummy;
	)

	i = row1[len2];
	free(row0);
	free(row1);
	free(row2);

	R i;
}