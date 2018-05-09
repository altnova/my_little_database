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

//! remove more
#define SW switch
#define CD default
#define CS(n,x)	case n:x;break;	//< never forget break again

//! fail fast
#define P(x,y) {if(x)R(y);}	//< panic
#define X(ex,catch) {P(ex,(({catch}),NIL));} //< throw

//! no stinking loops
#define DO(n,x) {UJ i=0,_i=(n);for(;i<_i;++i){x;}}
#define W(x) while((x))

#define MIN(x,y) ((y)>(x)?(x):(y))
#define MAX(x,y) ((y)>(x)?(y):(x))
#define IN(l,x,r) ((l)<=(x)&&(x)<=(r))

//! secure copy max n bytes from y into x \param x dststr \param y srcstr \param n limit
#define scpy_s(x,y,n) (S)memcpy((S)(x),(S)(y),MIN(scnt((S)y),n))

//! usual suspects
#define scnt(x)   (UJ)strlen((S)(x))		//!< string length \param str
#define scmp(x,y) strcmp((S)(x),(S)(y))     //!< compare two strings \param x str \param y str
#define scpy(x,y) (S)strcpy((S)(x),(S)(y))  //!< copy x into y \param x srcstr \param y deststr
#define schr(x,y) (S)strchr((S)(x),y)       //!< pointer to the first occurrence of y in x
#define rchr(x,y) (S)strrchr((S)(x),y)      //!< pointer to the last occurrence of y in x

//! zap spaces
#define ZV Z V          //!< static void
#define ZI Z I          //!< static int



//:~