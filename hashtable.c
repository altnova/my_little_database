#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "books.h"

#define SIZE 32

struct bucket{I h,n;struct bucket*next;I usage;C s[];};
Z I split,level=2;
Z struct bucket **buckets;

//! djbhash
I hC(G*a,UI n){I h=5381;DO(n,h=33*(h^a[i]))R h;}  // djb2 https://groups.google.com/forum/#!topic/comp.lang.c/lSKWXiuNOAk

//! check memory limit
Z G*chk(G*a){if(!a)O("out of memory\n"),exit(1);R a;}

//! fast binary copy \param d dest \param s source \param n len
G*dsn(V*d,V*s,UJ n){R(S)memcpy(d,s,n)+n;}

Z UJ S1; //< syms size
Z UI j;  //< total syms

//! add sym \param s sym \param n sym length \see http://en.wikipedia.org/wiki/Hash_table#Incremental_resizing */
// https://svn.process-one.net/ejabberd/tags/ejabberd-2.0.5/src/tls/tls_drv.c
S _snn(S s){
 I n = strlen(s);
 struct bucket*b;
 I i,hash=hC(s,n),bucket=hash&(level-1);
 O("h=%d b=%d\n", hash, bucket);
 I hi = 0;
 if(bucket<split) {
  hi = 1;
  O("********** bucket(%d)<split(%d) -> b=%d\n", bucket, split, hash&((level<<1)-1));
  bucket=hash&((level<<1)-1);
 }
 b=buckets[bucket];
 while(b){								//< search sym in bucket
  if(b->n==n){							//< if length matches
   DO(n,if(b->s[i]!=s[i])goto L0)		//< compare bytes, L0 if no match
   R b->s;}								//< found a match
  L0:b=b->next;}						//< move to next bucket element

 j++;									//< sym not found, create it
 O("_sn %s %d\tbucket=%d, split=%d, level=%d ",hi?"HI":"LO",j,bucket,split,level);
 //DO(n,O("%c", s[i]));O("\n");

 S1+=i=sizeof(struct bucket)+n+1;
 b=(struct bucket*)chk(malloc(i));
 b->h=hash;
 b->n=n;
 b->usage=-1;
 *dsn(b->s,s,n)=0;
 b->next=buckets[bucket];
 buckets[bucket]=b;
 if(b->next){
  DO(3,
   struct bucket**bp=&buckets[split];
   I newBucket=split+level;
   while(*bp){
    if(((*bp)->h&((level<<1)-1))==newBucket){
     struct bucket*tmp=*bp;
     *bp=(*bp)->next;
     tmp->next=buckets[newBucket];
     buckets[newBucket] = tmp;}
     else bp=&(*bp)->next;
   }
   if(++split==level){
    level<<=1;
    split=0;
    buckets=(struct bucket**)chk(realloc((G*)buckets,SIZE*2*(UJ)level));
    O("*********** hm realloc %d\n", SIZE*2*(UJ)level);
    DO(level,buckets[level+i]=0)}
  )
 }
 R b->s;}

I main() {

 buckets=(struct bucket**)calloc(2*level,SIZE);  //< initialize hash table

 _snn("FKTABLE_CAT");
 _snn("cov");
 _snn("bmp");
 _snn("frameset");
 _snn("cos");
 _snn("fmt");
 
 struct bucket *b;
 UJ c=0;
 DO(level*2,
  b=buckets[i];
  if(b)O("\n");
  while(b){
   O("%d %s ", i, b->s);
   b=b->next;c++;}
 );
 O("\n=%d\n",c);

}
