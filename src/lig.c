//! test fira code retina ligatures
V*;
char*;
*asdf;

S syntext() {
	fn(*(*asdf),a);
	**asdf;
	a,&a;
	a,**a;
	(a)a;
	asdf(*x);
	LOG("asdf");

	I* asdf; I *asdf;	
	char *asdf; char *c;
	int* asdf; (int*)asdf;
	FILE *fp; FILE*fp;
	(V*)(asdf);
	
	*asdf;
	asfd*asdf;
	(asdf + *asdf);
	*(asdf+asdf);
	(*asdf);
	&asdf[*asdf];
	*(*(*(*asdf)));
	&(&(&(&asdf)));
	&asdf;
	&(asdf+43);
	x[&abc];
	asdf&asdf;
	2*2;
}

V* ligtest() {
	/* hinted comment tags */ 
	// compressed double slash

	x->asdf; // accessors!

	if(x==y||x!=y||x&&y||x>=y||x<=y){
		x++; x--; !!x; !x; ~x;
		x^y; x%y; x&y;
		x<<=1; y>>=2;
		x?1:2;
	}
}
