//! \file ___.h \brief type less <=> do more

#define _GNU_SOURCE
#include <stddef.h>
#include <stdio.h>

//! type system
typedef char C;
typedef char* S;
typedef int I;
typedef short H;
typedef void V;
typedef float E;
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

//! better switch
#define SW switch
#define CD default
#define CS(n,x)	case n:x;break;	//< never forget a break again

//! fail fast
#define P(x,y) {if(x)R(y);}	//< panic
#define X(x,y,z) {if(x){(y);R(z);}} //< clean up, then panic

//! no stinking loops
#define DO(n,x) {UJ i=0,_i=(n);for(;i<_i;++i){x;}}
#define W(x) while((x))

//! comparisons done right
#define ABS(x) (((x)>0)?(x):-(x))
#define MIN(x,y) ((y)>(x)?(x):(y))
#define MAX(x,y) ((y)>(x)?(y):(x))
#define IN(l,x,r) ((l)<=(x)&&(x)<=(r))

//! usual suspects
#define scnt(x) (UJ)strlen((S)(x))				/*!< string length \param str */
#define scmp(x,y) strcmp((S)(x),(S)(y))     	/*!< compare two strings \param x str \param y str */
#define schr(h,n) (S)strchr((S)(h),n)       	/*!< first pos \param h haystack \param n needle */
#define rchr(h,n) (S)strrchr((S)(h),n)      	/*!< last pos \param h haystack \param n needle */
#define mcpy(d,s,n) memcpy((d),(s),n) 			/*!< mem copy \param d dest \param s source \param n len */
#define mcmp(d,s,n) memcmp((d),(s),n) 			/*!< mem compare \param d dest \param s source \param n len */
#define scpy(d,s,n) (S)mcpy((S)d,(S)s,1+MIN(scnt((S)s),n)) /*!< better strcpy \param d dest \param s source \param n limit */
#define lcse(s,n) {DO(n,s[i]=tolower(s[i]))}
#define stok(s,n,d,x) {ZC z=0,D[255]="*";if(!z){z=1;DO(scnt(d),D[d[i]]='*')}/*!< tokenize \param s,n,d,x string,len,delims,exec */\
					   I in=0;sz tok_len,tok_pos=0;S tok;DO(n+1,/*if(!IN(' ',s[i],'~'))O("(%d=%d)",i,s[i]);*/\
					   if(D[s[i]]){if(in){in=0;tok=s+tok_pos;s[i]='\0';tok_len=i-tok_pos;{x;}}\
					   else tok_pos=i;}else if(!in){in=1;tok_pos=i;})}

//! less is more
#define ZV Z V
#define ZI Z I
#define ZC Z C
#define ZS Z S
#define ZE Z E
#define ZJ Z J
#define R0 R 0
#define R1 R 1

//! error
#define NIL 0xffffffffffffffff

//:~
