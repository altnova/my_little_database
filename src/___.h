//!\file ___.h \brief core macros

#define _GNU_SOURCE
#include <stddef.h>
#include <stdio.h>

#define NIL 0xffffffffffffffff

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
typedef size_t sz;

//! remove clutter
#define R return
#define O printf
#define SZ sizeof
#define Z static
#define ext extern

//! fix switch
#define SW switch
#define CD default
#define CS(n,x)	case n:x;break;	//< never forget a break again

//! fail fast
#define P(x,y) {if(x)R(y);}	//< panic early
#define X(x,y,z) {if(x){y;R(z);}} //< clean up and throw nil

//! no stinking loops
#define DO(n,x) {UJ i=0,_i=(n);for(;i<_i;++i){x;}}
#define W(x) while((x))

#define MIN(x,y) ((y)>(x)?(x):(y))
#define MAX(x,y) ((y)>(x)?(y):(x))
#define IN(l,x,r) ((l)<=(x)&&(x)<=(r))

//! secure strcpy

//! usual suspects
#define scnt(x)   (UJ)strlen((S)(x))		//!< string length \param str
#define scmp(x,y) strcmp((S)(x),(S)(y))     //!< compare two strings \param x str \param y str
#define schr(h,n) (S)strchr((S)(h),n)       //!< first occurrence of needle in haystack
#define rchr(h,n) (S)strrchr((S)(h),n)      //!< last occurrence of needle in haystack
#define scpy(d,s,n) (S)memcpy((S)(d),(S)(s),MIN(scnt((S)s),n)) //!< \param d dest \param s source \param n
//#define scpy(d,s) (S)strcpy((S)(d),(S)(s))  //!< free buffer overflows \param d dest \param s source

//! zap spaces
#define ZV Z V          //!< static void
#define ZI Z I          //!< static int


//:~