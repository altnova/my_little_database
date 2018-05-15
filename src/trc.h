//!\file trc.h \brief logging system api

enum loglevels { L_FATAL, L_WARN, L_INFO, L_TEST, L_DEBUG, L_TRACE };

//! register function for logging
#define LOG(fn) const Z S FN=fn;

#define STR_ARROW_RIGHT "\e[37m->\e[0m"
#define STR_EMPTY_SET "\e[91m∅\e[0m"

#define FATAL L_FATAL,FN,__FILE__,__LINE__
#define WARN L_WARN,FN,__FILE__,__LINE__
#define INFO L_INFO,FN,__FILE__,__LINE__
#define TEST L_TEST,FN,__FILE__,__LINE__
#define DEBUG L_DEBUG,FN,__FILE__,__LINE__
#define TRACE L_TRACE,FN,__FILE__,__LINE__

ZS loglevel_names[] = {"FATAL", "WARN", "INFO", "TEST", "DEBUG", "TRACE"};

//! trace
ext I T(I lvl, const S fn, const S file, const I line, const S format, ...);
ext V TSTART(), TEND();

//! check malloc ptr
#define chk(a,retval) X(!a,T(FATAL,"out of memory"),retval);

//! bitwise
ext V bits_char(C x, S dest);

//:~