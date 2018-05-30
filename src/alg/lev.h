//! \file lev.h \brief levenshtein distance

ext I lev(const S string1, const S string2,
	I swap_penalty, I substitution_penalty,
	I insertion_penalty, I deletion_penalty);
