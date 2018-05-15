//! \file rnd.c \brief randomness api

enum charsets { CHARSET_ALNUM, CHARSET_AZaz, CHARSET_AZ, CHARSET_az};

//! random string
//! \param dest,len destination, length
//! \param charset CHARSET_ALNUM|CHARSET_AZaz|CHARSET_AZ|CHARSET_az
//! \return ptr to \c dest
S rnd_str(S dest, sz size, I charset);