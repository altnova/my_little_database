

#define LOG(fn) const Z S FN=fn;
#define WARN L_WARN,FN,__FILE__,__LINE__
#define INFO L_INFO,FN,__FILE__,__LINE__
#define TEST L_TEST,FN,__FILE__,__LINE__
#define DEBUG L_DEBUG,FN,__FILE__,__LINE__
#define TRACE L_TRACE,FN,__FILE__,__LINE__

//! trace
extern V T(I lvl, S fn, S file, I line, S format, ...);

//:~