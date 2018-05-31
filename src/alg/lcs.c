//! \file lcs.c \brief longest common subsequence

#include <stdio.h>
#include <string.h>
#include "../___.h"
 
I i,j,m,n,c[20][20];
C x[20],y[20],b[20][20];
 
I out(I i,I j){
	P(i==0||j==0,0)
	if(b[i][j]=='c'){
		out(i-1,j-1);
		O("%c",x[i-1]);
	}else if(b[i][j]=='u')
		out(i-1,j);
	else
		out(i,j-1);
	R0;}
 
V lcs(){
	m=scnt(x);
	n=scnt(y);
	for(i=0;i<=m;++i)c[i][0]=0;
	for(i=0;i<=n;++i)c[0][i]=0;	
	//c,u,l cross,up,down
	for(i=1;i<=m;i++)
		for(j=1;j<=n;j++) {
			if(x[i-1]==y[j-1]){
				c[i][j]=c[i-1][j-1]+1;
				b[i][j]='c';
			}
			else if(c[i-1][j]>=c[i][j-1]){
				c[i][j]=c[i-1][j];
				b[i][j]='u';
			}
			else{
				c[i][j]=c[i][j-1];
				b[i][j]='d';
			}
		}
}
 
I main(){
	scpy(x,"vladimir",8);
	scpy(y,"trldmizz",8);
	lcs();
	O("lcs: ");
	out(m,n);
	R0;}
