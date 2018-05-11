//! test fira code retina ligatures
V*;
char*;
*asdf;

V* ligtest() {
	asdf(*(*asdf),a);
	**asdf;
	a,&a;
	a,**a;
	
	asdf(*x);
	LOG("asdf");
	char *aasdf;
	I* asdf;
	I *asdf;
	int* asdf;
	(int*)asdf;
	(V*)(asdf);
	FILE *fp;
	FILE*fp;
	char *c;
	2*2;
	*asdf;
	asfd*asdf;
	(asdf + *asdf);
	*(asdf+asdf);
	(*asdf);
	asdf[*asdf];
	*(*(*(*asdf)));
	&asdf;
	&(asdf);
	[&asdf];
	asdf&asdf;

	*asdf;

	/* hinted comment tags */ 
	// compressed double slash

	x->asdf; // accessors!

	if(x==y||x!=y||x&&y||x>=y||x<=y){} //< comparison
	++x; --x; x++; x--; !!x; !x; ~x;	//< good idea to highlight these guys
	x^y;
	x||y;
	x%y;
	x&y;
	x?1:2;
	x<<=1;
	y>>=2;
}
