#include <stddef.h>
#include <stdio.h>

enum loglevels { L_WARN, L_INFO, L_TEST, L_DEBUG, L_TRACE };

#define LOG(fn) const Z S FN=fn;
#define WARN L_WARN,FN,__FILE__,__LINE__
#define INFO L_INFO,FN,__FILE__,__LINE__
#define TEST L_TEST,FN,__FILE__,__LINE__
#define DEBUG L_DEBUG,FN,__FILE__,__LINE__
#define TRACE L_TRACE,FN,__FILE__,__LINE__

//! type system
typedef char C;
typedef char* S;
typedef int I;
typedef short H;
typedef void V;
typedef unsigned long UJ;
typedef long J;
typedef unsigned int UI;
typedef unsigned char G;

//! remove clutter
#define Z static
#define R return
#define O printf
#define SZ sizeof

//! remove more clutter
#define SW switch
#define CD default
#define CS(n,x)	case n:x;break;

//! no stinking loops
#define DO(n,x) {UJ i=0,_i=(n);for(;i<_i;++i){x;}}

#define MIN(x,y) ((y)>(x)?(x):(y))
#define MAX(x,y) ((y)>(x)?(y):(x))

//! string manipulation
#define scnt(x)   (UJ)strlen((S)(x))		//!< string length \param str
#define scmp(x,y) strcmp((S)(x),(S)(y))     //!< compare two strings \param x str \param y str
#define scpy(x,y) (S)strcpy((S)(x),(S)(y))  //!< copy x into y \param x srcstr \param y deststr
#define scpy_s(x,y,n) (S)memcpy((S)(x),(S)(y),MIN(scnt((S)y),n))  //!< secure copy max n bytes from y into x \param x dststr \param y srcstr \param n limit
#define schr(x,y) (S)strchr((S)(x),y)       //!< pointer to the first occurrence of y in x
#define rchr(x,y) (S)strrchr((S)(x),y)      //!< pointer to the last occurrence of y in x

//:~