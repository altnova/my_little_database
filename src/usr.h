//! \file usr.h \brief user input

#define LINE_BUF 2048
#define USR_LOOP(x,y) while(!x){y;}

//! read integer from the console into \c num
//! \param prompt prompt text
//! \param prompt text to display on parse error
//! \return 0 on success, EOF on error
ext I usr_input_num(I* num, S prompt, S errmsg);

//! read string from the console into \c str
//! \param prompt prompt text
//! \param prompt text to display on parse error
//! \return 0 on success, EOF on error
ext I usr_input_str(S str, S prompt, S errmsg);
